#include "WebSocket.h"

#include <cstdint>
#include <mutex>

#include "../utils/Base64.h"
#include "../utils/Sha1.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using sock_t = SOCKET;
#else
#include <sys/socket.h>
#include <unistd.h>
using sock_t = int;
#endif

namespace ws {
namespace {

// Este GUID es un valor fijo que define el estandar RFC 6455. El servidor lo
// concatena con la clave del cliente antes de calcular SHA-1. No es una clave
// secreta, solo sirve para demostrar que el servidor conoce el protocolo.
const char* GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

// Un mensaje WebSocket no deberia crecer sin limite. Con este valor evitamos
// que un cliente mal intencionado reserve memoria de forma descontrolada.
const uint64_t MAX_MENSAJE = 2u * 1024u * 1024u;

// Un mismo socket puede recibir un ping o un cierre desde un hilo mientras
// otro hilo esta enviando un mensaje. Si las tramas se mezclan, el navegador
// recibe bytes que no puede interpretar. Este mutex hace que las escrituras se
// hagan una por una.
std::mutex envioMtx;

// TCP es un flujo de bytes, no una lista de mensajes. Una llamada a send puede
// enviar solo una parte de los datos, por eso repetimos hasta terminar.
bool enviarTodo(sock_t s, const char* datos, size_t len) {
    size_t enviado = 0;
    while (enviado < len) {
#ifdef _WIN32
        int n = send(s, datos + enviado, static_cast<int>(len - enviado), 0);
#else
        ssize_t n = send(s, datos + enviado, len - enviado, 0);
#endif
        if (n <= 0) return false;
        enviado += static_cast<size_t>(n);
    }
    return true;
}

// La lectura tiene el mismo problema que la escritura. recv puede devolver
// menos bytes de los que pedimos, asi que repetimos hasta completar el tamano
// exacto. Devuelve 1 si leyo todo, 0 si el socket se cerro y -1 si hubo error.
int leerTodo(sock_t s, char* datos, size_t len) {
    size_t leido = 0;
    while (leido < len) {
#ifdef _WIN32
        int n = recv(s, datos + leido, static_cast<int>(len - leido), 0);
#else
        ssize_t n = recv(s, datos + leido, len - leido, 0);
#endif
        if (n == 0) return 0;
        if (n < 0) return -1;
        leido += static_cast<size_t>(n);
    }
    return 1;
}

// Una trama WebSocket se arma en dos partes. La cabecera dice que tipo de
// mensaje es y cuanto mide. El payload lleva los bytes del mensaje. El primer
// byte de la cabecera tiene el bit FIN, que indica que la trama es la ultima,
// y el opcode, que dice si es texto, binario, cierre, ping o pong. El segundo
// byte tiene el bit de mascara y el tamano. Como el servidor no enmascara sus
// tramas, ese bit queda en cero y no agregamos los cuatro bytes de mascara.
bool enviarTrama(sock_t s, unsigned char opcode, const std::string& payload) {
    std::lock_guard<std::mutex> lock(envioMtx);
    std::string trama;
    trama.reserve(payload.size() + 10);

    // Ponemos el bit FIN en uno porque enviamos el mensaje completo en una sola
    // trama. El opcode ocupa los cuatro bits menos significativos.
    trama.push_back(static_cast<char>(0x80 | opcode));

    // El tamano tiene tres formatos. Si el payload mide 125 bytes o menos, el
    // tamano se escribe directo en los siete bits del segundo byte. Si no, se
    // escribe 126 y los siguientes dos bytes guardan el tamano. Para mensajes
    // muy grandes se escribe 127 y los siguientes ocho bytes guardan el tamano
    // en orden big-endian.
    uint64_t n = payload.size();
    if (n <= 125) {
        trama.push_back(static_cast<char>(n));
    } else if (n <= 65535) {
        trama.push_back(static_cast<char>(126));
        trama.push_back(static_cast<char>((n >> 8) & 0xFF));
        trama.push_back(static_cast<char>(n & 0xFF));
    } else {
        trama.push_back(static_cast<char>(127));
        for (int i = 7; i >= 0; i--)
            trama.push_back(static_cast<char>((n >> (i * 8)) & 0xFF));
    }
    trama += payload;
    return enviarTodo(s, trama.data(), trama.size());
}

}  // namespace

// El handshake de WebSocket no usa una clave secreta. El cliente manda una
// clave aleatoria en Sec-WebSocket-Key y el servidor debe demostrar que entendio
// el protocolo devolviendo un valor calculado. Primero unimos la clave con el
// GUID fijo, despues calculamos SHA-1 y al final codificamos el resultado en
// Base64. Ese texto es el que viaja en Sec-WebSocket-Accept.
std::string acceptKey(const std::string& clientKey) {
    std::string crudo = sha1::digest(clientKey + GUID);
    return b64::encode(crudo);
}

// El opcode 0x1 identifica una trama de texto. El servidor envia el mensaje
// completo y el navegador lo entrega como string.
bool sendText(unsigned long long sock, const std::string& texto) {
    return enviarTrama(static_cast<sock_t>(sock), 0x1, texto);
}

// Cuando el cliente manda un ping, el servidor debe responder con un pong que
// lleve el mismo payload. Esto sirve para comprobar que la conexion sigue viva.
bool sendPong(unsigned long long sock, const std::string& payload) {
    return enviarTrama(static_cast<sock_t>(sock), 0xA, payload);
}

// El opcode 0x8 identifica el cierre. El payload lleva un codigo de dos bytes.
// Usamos 1000, que significa cierre normal, para que el navegador sepa que no
// hubo un error de protocolo.
bool sendClose(unsigned long long sock) {
    std::string payload;
    payload.push_back(static_cast<char>(0x03));
    payload.push_back(static_cast<char>(0xE8));
    return enviarTrama(static_cast<sock_t>(sock), 0x8, payload);
}

// Leemos tramas hasta completar un mensaje. Un mensaje puede llegar en una
// sola trama o partido en varias. La primera trama trae el opcode 0x1 para
// texto o 0x2 para binario. Las siguientes traen el opcode 0x0, que significa
// continuacion. Cuando llega una trama con el bit FIN encendido, el mensaje
// esta completo y lo devolvemos.
int recvMessage(unsigned long long sock, std::string& salida) {
    sock_t s = static_cast<sock_t>(sock);
    std::string mensaje;
    bool iniciado = false;

    while (true) {
        // Toda trama empieza con dos bytes de cabecera. Si el socket se cerro
        // antes de leerlos, devolvemos 0. Si hubo un error, devolvemos -1.
        unsigned char cabecera[2];
        int estado = leerTodo(s, reinterpret_cast<char*>(cabecera), 2);
        if (estado <= 0) return estado;

        // El bit mas alto del primer byte es FIN. Los cuatro bits bajos son el
        // opcode. En el segundo byte, el bit mas alto dice si el payload viene
        // enmascarado y los siete bits restantes guardan el tamano.
        bool fin = (cabecera[0] & 0x80) != 0;
        unsigned char opcode = cabecera[0] & 0x0F;
        bool enmascarado = (cabecera[1] & 0x80) != 0;
        uint64_t longitud = cabecera[1] & 0x7F;

        // Si el tamano no cabe en siete bits, la cabecera se alarga. El valor
        // 126 indica que siguen dos bytes y el valor 127 indica que siguen
        // ocho bytes. Los bytes llegan en orden big-endian.
        if (longitud == 126) {
            unsigned char ext[2];
            if (leerTodo(s, reinterpret_cast<char*>(ext), 2) <= 0) return -1;
            longitud = (static_cast<uint64_t>(ext[0]) << 8) | ext[1];
        } else if (longitud == 127) {
            unsigned char ext[8];
            if (leerTodo(s, reinterpret_cast<char*>(ext), 8) <= 0) return -1;
            longitud = 0;
            for (int i = 0; i < 8; i++)
                longitud = (longitud << 8) | ext[i];
        }

        // Rechazamos mensajes demasiado grandes para no llenar la memoria.
        if (longitud > MAX_MENSAJE) return -1;

        // El estandar obliga a que el cliente enmascare todas sus tramas. Si
        // llega una trama sin mascara, la conexion no sigue el protocolo.
        if (!enmascarado) return -1;

        // La mascara son cuatro bytes. Cada byte del payload se combina con
        // uno de esos bytes usando XOR. El mismo procedimiento se repite cada
        // cuatro posiciones hasta desemmascarar todo.
        unsigned char mascara[4];
        if (leerTodo(s, reinterpret_cast<char*>(mascara), 4) <= 0) return -1;

        std::string payload(static_cast<size_t>(longitud), '\0');
        if (longitud > 0) {
            if (leerTodo(s, &payload[0], static_cast<size_t>(longitud)) <= 0) return -1;
            for (size_t i = 0; i < payload.size(); i++)
                payload[i] = static_cast<char>(static_cast<unsigned char>(payload[i]) ^
                                               mascara[i % 4]);
        }

        // El opcode 0x8 pide cerrar la conexion. Respondemos con un cierre
        // normal y devolvemos 0 para que el hilo del chat termine.
        if (opcode == 0x8) {
            sendClose(sock);
            return 0;
        }
        // El opcode 0x9 es un ping. Respondemos con un pong que lleva el mismo
        // payload y seguimos esperando el siguiente mensaje.
        if (opcode == 0x9) {
            sendPong(sock, payload);
            continue;
        }
        // El opcode 0xA es un pong. No necesitamos hacer nada con el, solo
        // continuar con la siguiente trama.
        if (opcode == 0xA) continue;

        // Las tramas de texto o binarias inician un mensaje nuevo. Si ya
        // habiamos iniciado uno, la secuencia esta mal armada.
        if (opcode == 0x1 || opcode == 0x2) {
            if (iniciado) return -1;
            mensaje = payload;
            iniciado = true;
        } else if (opcode == 0x0) {
            // Una trama de continuacion agrega su payload al mensaje que ya
            // estamos armando. Antes comprobamos que no supere el limite.
            if (!iniciado) return -1;
            if (mensaje.size() + payload.size() > MAX_MENSAJE) return -1;
            mensaje += payload;
        } else {
            // Cualquier otro opcode no esta soportado por este servidor.
            return -1;
        }

        // Si esta trama trae FIN, el mensaje esta completo. Lo copiamos en
        // salida y devolvemos 1.
        if (fin) {
            if (!iniciado) return -1;
            salida = mensaje;
            return 1;
        }
    }
}

}  // namespace ws
