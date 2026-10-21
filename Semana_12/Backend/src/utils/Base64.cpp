#include "Base64.h"

namespace b64 {

// Base64 trabaja con grupos de tres bytes. Cada grupo tiene 24 bits y se
// reparte en cuatro caracteres de 6 bits. Por eso el resultado siempre crece
// en bloques de cuatro caracteres. Si el ultimo grupo no tiene tres bytes,
// rellenamos con cero y marcamos los caracteres que faltan con el signo igual.
std::string encode(const std::string& bytes) {
    static const char* T = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve(((bytes.size() + 2) / 3) * 4);
    for (size_t i = 0; i < bytes.size(); i += 3) {
        // Tomamos hasta tres bytes. Si el grupo esta incompleto, los bytes que
        // faltan valen cero y no se escriben en la salida.
        unsigned a = static_cast<unsigned char>(bytes[i]);
        unsigned b = (i + 1 < bytes.size()) ? static_cast<unsigned char>(bytes[i + 1]) : 0;
        unsigned c = (i + 2 < bytes.size()) ? static_cast<unsigned char>(bytes[i + 2]) : 0;
        unsigned n = (a << 16) | (b << 8) | c;
        // Cada caracter toma seis bits del grupo de 24 bits.
        out += T[(n >> 18) & 63];
        out += T[(n >> 12) & 63];
        out += (i + 1 < bytes.size()) ? T[(n >> 6) & 63] : '=';
        out += (i + 2 < bytes.size()) ? T[n & 63] : '=';
    }
    return out;
}

}  // namespace b64
