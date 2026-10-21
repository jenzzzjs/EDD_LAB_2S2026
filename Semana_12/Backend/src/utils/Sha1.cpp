#include "Sha1.h"

#include <cstdint>
#include <cstring>

namespace sha1 {
namespace {

// Una rotacion a la izquierda mueve los bits hacia la posicion mas alta y
// vuelve a meter los que se salieron por la derecha. SHA-1 la usa en varias
// partes del calculo para mezclar los bits del mensaje.
inline uint32_t rotarIzquierda(uint32_t valor, unsigned bits) {
    return (valor << bits) | (valor >> (32 - bits));
}

}  // namespace

// SHA-1 trabaja con bloques de 64 bytes. Primero rellenamos el mensaje para
// que su tamano sea multiplo de 64. Agregamos un byte 0x80, despues ceros y al
// final la longitud original en bits. Ese relleno es parte del algoritmo y no
// se guarda en ningun archivo.
std::string digest(const std::string& entrada) {
    // Estos cinco valores son el estado inicial que define el estandar.
    uint32_t h0 = 0x67452301u;
    uint32_t h1 = 0xEFCDAB89u;
    uint32_t h2 = 0x98BADCFEu;
    uint32_t h3 = 0x10325476u;
    uint32_t h4 = 0xC3D2E1F0u;

    size_t longitud = entrada.size();
    // Calculamos cuantos bloques de 64 bytes necesitamos. Siempre reservamos
    // espacio para el byte 0x80 y para los 8 bytes de la longitud.
    size_t total = ((longitud + 8) / 64 + 1) * 64;
    unsigned char* mensaje = new unsigned char[total]();
    for (size_t i = 0; i < longitud; i++)
        mensaje[i] = static_cast<unsigned char>(entrada[i]);
    mensaje[longitud] = 0x80;

    uint64_t bits = static_cast<uint64_t>(longitud) * 8;
    for (int i = 0; i < 8; i++)
        mensaje[total - 1 - i] = static_cast<unsigned char>((bits >> (i * 8)) & 0xFF);

    // Procesamos el mensaje bloque por bloque. Cada bloque se convierte en 80
    // palabras de 32 bits. Las primeras 16 salen directamente de los bytes y
    // las otras 64 se calculan con XOR y rotaciones.
    for (size_t bloque = 0; bloque < total; bloque += 64) {
        uint32_t w[80];
        for (int i = 0; i < 16; i++) {
            const unsigned char* p = mensaje + bloque + i * 4;
            w[i] = (static_cast<uint32_t>(p[0]) << 24) |
                   (static_cast<uint32_t>(p[1]) << 16) |
                   (static_cast<uint32_t>(p[2]) << 8) |
                   static_cast<uint32_t>(p[3]);
        }
        for (int i = 16; i < 80; i++)
            w[i] = rotarIzquierda(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

        // Copiamos el estado en cinco variables de trabajo. En cada una de las
        // 80 vueltas mezclamos esas variables con una funcion distinta y una
        // constante distinta. Al final volvemos a sumar el resultado al estado.
        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int i = 0; i < 80; i++) {
            uint32_t f = 0;
            uint32_t k = 0;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999u;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1u;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDCu;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6u;
            }
            uint32_t temp = rotarIzquierda(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = rotarIzquierda(b, 30);
            b = a;
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    delete[] mensaje;

    // El digest se arma con los cinco valores finales en orden big-endian. El
    // resultado son exactamente 20 bytes, que es lo que Base64 convierte en
    // texto para el handshake.
    std::string salida(20, '\0');
    uint32_t estado[5] = {h0, h1, h2, h3, h4};
    for (int i = 0; i < 5; i++) {
        salida[i * 4 + 0] = static_cast<char>((estado[i] >> 24) & 0xFF);
        salida[i * 4 + 1] = static_cast<char>((estado[i] >> 16) & 0xFF);
        salida[i * 4 + 2] = static_cast<char>((estado[i] >> 8) & 0xFF);
        salida[i * 4 + 3] = static_cast<char>(estado[i] & 0xFF);
    }
    return salida;
}

}  // namespace sha1
