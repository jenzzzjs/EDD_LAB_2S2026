#include "ChatController.h"

#include <mutex>

#include "../http/WebSocket.h"
#include "../models/ChatStore.h"
#include "../models/Store.h"
#include "../utils/Json.h"

namespace ChatController {
namespace {

// Cada objeto ClienteWs representa una conexion WebSocket viva. Un mismo
// usuario puede tener varias pestanas abiertas, por eso guardamos el socket y
// el correo de cada conexion por separado.
struct ClienteWs {
    unsigned long long sock;
    std::string correo;
};

// El ChatHub es el registro de conexiones WebSocket que estan activas en este
// momento. Cuando un usuario abre el chat, su socket se agrega al hub. Cuando
// cierra la pestana, se quita. Para enviar un mensaje en tiempo real buscamos
// todas las conexiones del destinatario y escribimos en cada una.
class ChatHub {
public:
    static ChatHub& get() {
        static ChatHub instancia;
        return instancia;
    }

    // Agregamos una conexion nueva. Si el socket ya estaba registrado no lo
    // duplicamos, porque eso haria que el usuario recibiera dos veces el mismo
    // evento.
    void agregar(unsigned long long sock, const std::string& correo) {
        std::lock_guard<std::mutex> lock(mtx_);
        for (size_t i = 0; i < clientes_.tam(); i++)
            if (clientes_[i].sock == sock) return;
        clientes_.push_back(ClienteWs{sock, correo});
    }

    // Cuando una conexion termina, la sacamos del registro. Copiamos las que
    // siguen vivas a una lista nueva para no modificar la lista original
    // mientras la estamos recorriendo.
    void quitar(unsigned long long sock) {
        std::lock_guard<std::mutex> lock(mtx_);
        Lista<ClienteWs> nueva;
        for (size_t i = 0; i < clientes_.tam(); i++)
            if (clientes_[i].sock != sock) nueva.push_back(clientes_[i]);
        clientes_ = std::move(nueva);
    }

    // Enviamos un evento a todas las pestanas de un correo. Primero copiamos
    // los sockets bajo el mutex del registro y despues enviamos fuera de ese
    // mutex. Asi no bloqueamos el registro mientras una escritura de red puede
    // tardar.
    void enviarA(const std::string& correo, const std::string& evento) {
        Lista<unsigned long long> destinos;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            for (size_t i = 0; i < clientes_.tam(); i++)
                if (clientes_[i].correo == correo) destinos.push_back(clientes_[i].sock);
        }
        if (destinos.vacia()) return;
        std::lock_guard<std::mutex> envio(envioMtx_);
        for (size_t i = 0; i < destinos.tam(); i++)
            ws::sendText(destinos[i], evento);
    }

    // Algunos eventos van solo a la conexion que los pidio, como la
    // bienvenida, un error de validacion o la respuesta de historial. Para eso
    // usamos el socket directo en lugar de buscar por correo.
    void enviarSocket(unsigned long long sock, const std::string& evento) {
        std::lock_guard<std::mutex> envio(envioMtx_);
        ws::sendText(sock, evento);
    }

private:
    std::mutex mtx_;
    std::mutex envioMtx_;
    Lista<ClienteWs> clientes_;
};

// Los eventos que viajan por WebSocket son texto JSON. Este ayudante arma un
// evento de error con un campo tipo y un mensaje legible para el frontend.
std::string eventoError(const std::string& mensaje) {
    return "{\"tipo\":\"error\",\"error\":" + json::str(mensaje) + "}";
}

// Convertimos un Mensaje a JSON para poder enviarlo por WebSocket o dentro de
// la respuesta de historial. Los campos conservan el mismo nombre que usa el
// frontend para que la interfaz los pueda leer sin traducciones.
std::string mensajeJson(const Mensaje& m) {
    return "{\"id\":" + json::str(m.id) + ",\"de\":" + json::str(m.de) +
           ",\"para\":" + json::str(m.para) + ",\"texto\":" + json::str(m.texto) +
           ",\"hora\":" + json::str(m.hora) + "}";
}

// La interfaz muestra cuanto espacio se ahorro con LZW. El porcentaje se
// calcula con los bytes del texto original y los bytes del archivo comprimido.
// Si el archivo esta vacio evitamos dividir entre cero y dejamos el ahorro en
// cero.
std::string lzwJson(const LzwInfo& info) {
    long ahorro = 0;
    if (info.bytesOriginal > 0) {
        double porcentaje = 100.0 - (static_cast<double>(info.bytesComprimido) * 100.0 /
                                     static_cast<double>(info.bytesOriginal));
        ahorro = static_cast<long>(porcentaje);
    }
    return "{\"archivo\":" + json::str(info.archivo) +
           ",\"bytes_original\":" + std::to_string(info.bytesOriginal) +
           ",\"bytes_comprimido\":" + std::to_string(info.bytesComprimido) +
           ",\"ahorro_porcentaje\":" + std::to_string(ahorro) + "}";
}

// Armamos el arreglo JSON de mensajes a mano. Recorremos la lista y separamos
// cada objeto con una coma, sin dejar una coma extra al final.
std::string arregloMensajes(const Lista<Mensaje>& mensajes) {
    std::string body = "[";
    for (size_t i = 0; i < mensajes.tam(); i++) {
        if (i > 0) body += ",";
        body += mensajeJson(mensajes[i]);
    }
    body += "]";
    return body;
}

// Aunque el frontend solo muestre amigos, el backend vuelve a comprobar que
// los dos usuarios existan y que el grafo tenga la arista de amistad. Esto
// evita que alguien mande un mensaje modificando la peticion a mano.
bool validarAmistad(const std::string& de, const std::string& para, int& status,
                    std::string& error) {
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    if (st.buscarUsuario(de) < 0 || st.buscarUsuario(para) < 0) {
        status = 404;
        error = "usuario no encontrado";
        return false;
    }
    if (de == para) {
        status = 400;
        error = "no puedes chatear contigo mismo";
        return false;
    }
    if (!st.grafo.sonAmigos(de, para)) {
        status = 403;
        error = "solo puedes chatear con amigos";
        return false;
    }
    return true;
}

}  // namespace

// El historial se pide por HTTP cuando el usuario abre una conversacion o
// cambia de amigo. El backend lee el archivo LZW de la pareja, lo descomprime y
// responde con los mensajes y los datos de compresion. Los mensajes nuevos no
// pasan por esta ruta, esos llegan por WebSocket.
Result historial(const std::string& correo, const std::string& amigo) {
    std::string c = toLowerStr(trimStr(correo));
    std::string a = toLowerStr(trimStr(amigo));
    if (c.empty() || a.empty())
        return {400, errJson("correo y amigo son obligatorios")};

    int status = 400;
    std::string error;
    if (!validarAmistad(c, a, status, error)) return {status, errJson(error)};

    ChatStore::ResultadoHistorial h = ChatStore::get().historial(c, a);
    if (!h.ok) return {500, errJson(h.error)};

    std::string body = "{\"ok\":true,\"correo\":" + json::str(c) + ",\"amigo\":" +
                       json::str(a) + ",\"mensajes\":" + arregloMensajes(h.mensajes) +
                       ",\"lzw\":" + lzwJson(h.lzw) + "}";
    return {200, body};
}

// Atendemos una conexion WebSocket desde el primer frame hasta que se cierra.
// La conexion ya viene identificada con el correo del usuario en la query. Si
// el usuario no existe, mandamos un error y terminamos. Si existe, lo
// registramos en el hub para que pueda recibir mensajes en tiempo real.
void websocket(unsigned long long sock, const std::string& correo) {
    std::string c = toLowerStr(trimStr(correo));
    bool existe = false;
    {
        Store& st = Store::get();
        std::lock_guard<std::mutex> lock(st.mtx);
        existe = st.buscarUsuario(c) >= 0;
    }
    if (!existe) {
        ws::sendText(sock, eventoError("usuario no encontrado"));
        return;
    }

    ChatHub& hub = ChatHub::get();
    hub.agregar(sock, c);
    hub.enviarSocket(sock, "{\"tipo\":\"conectado\",\"correo\":" + json::str(c) + "}");

    // Este ciclo se queda vivo mientras la pestana tenga la conexion abierta.
    // Cada vuelta lee un mensaje WebSocket completo, lo interpreta como JSON y
    // decide que hacer. Si recvMessage devuelve cero o un valor negativo, el
    // cliente cerro la conexion o hubo un error, y salimos del ciclo.
    while (true) {
        std::string payload;
        int estado = ws::recvMessage(sock, payload);
        if (estado <= 0) break;

        std::string tipo = json::getString(payload, "tipo");
        if (tipo == "mensaje") {
            // Para enviar un mensaje el frontend manda el destinatario y el
            // texto. Normalizamos los dos y rechazamos mensajes vacios o
            // demasiado largos antes de tocar el disco.
            std::string para = toLowerStr(trimStr(json::getString(payload, "para")));
            std::string texto = trimStr(json::getString(payload, "texto"));
            if (texto.empty()) {
                hub.enviarSocket(sock, eventoError("el mensaje esta vacio"));
                continue;
            }
            if (texto.size() > 2000) {
                hub.enviarSocket(sock, eventoError("el mensaje supera los 2000 caracteres"));
                continue;
            }

            // Volvemos a validar la amistad en el servidor. No basta con que
            // el frontend haya ocultado el boton de enviar.
            int status = 400;
            std::string error;
            if (!validarAmistad(c, para, status, error)) {
                hub.enviarSocket(sock, eventoError(error));
                continue;
            }

            // ChatStore guarda el mensaje comprimido con LZW y devuelve el
            // mensaje ya armado junto con los datos de compresion.
            ChatStore::ResultadoEnvio envio = ChatStore::get().enviar(c, para, texto);
            if (!envio.ok) {
                hub.enviarSocket(sock, eventoError(envio.error));
                continue;
            }

            // Enviamos el mismo evento al remitente y al destinatario. Al
            // remitente le sirve para confirmar que el mensaje se guardo. Al
            // destinatario le llega en tiempo real si tiene el chat abierto.
            std::string evento = "{\"tipo\":\"mensaje\",\"mensaje\":" +
                                 mensajeJson(envio.mensaje) + ",\"lzw\":" +
                                 lzwJson(envio.lzw) + "}";
            hub.enviarA(c, evento);
            hub.enviarA(para, evento);
        } else if (tipo == "historial") {
            // El frontend tambien puede pedir el historial por WebSocket. La
            // respuesta se manda solo a la conexion que hizo la solicitud.
            std::string amigo = toLowerStr(trimStr(json::getString(payload, "amigo")));
            int status = 400;
            std::string error;
            if (!validarAmistad(c, amigo, status, error)) {
                hub.enviarSocket(sock, eventoError(error));
                continue;
            }
            ChatStore::ResultadoHistorial h = ChatStore::get().historial(c, amigo);
            if (!h.ok) {
                hub.enviarSocket(sock, eventoError(h.error));
                continue;
            }
            std::string evento = "{\"tipo\":\"historial\",\"amigo\":" + json::str(amigo) +
                                 ",\"mensajes\":" + arregloMensajes(h.mensajes) +
                                 ",\"lzw\":" + lzwJson(h.lzw) + "}";
            hub.enviarSocket(sock, evento);
        } else if (tipo == "ping") {
            // El ping de la aplicacion viaja como un evento JSON. Respondemos
            // con un pong para que el frontend sepa que el servidor sigue vivo.
            hub.enviarSocket(sock, "{\"tipo\":\"pong\"}");
        } else {
            hub.enviarSocket(sock, eventoError("tipo de evento no reconocido"));
        }
    }

    // Al salir del ciclo quitamos la conexion del hub para no intentar enviarle
    // mensajes a un socket que ya esta cerrado.
    hub.quitar(sock);
}

}  // namespace ChatController
