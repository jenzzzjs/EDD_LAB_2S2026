#include "Base64.h"

namespace b64 {

std::string encode(const std::string& bytes) {
    static const char* T = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve(((bytes.size() + 2) / 3) * 4);
    for (size_t i = 0; i < bytes.size(); i += 3) {
        unsigned a = static_cast<unsigned char>(bytes[i]);
        unsigned b = (i + 1 < bytes.size()) ? static_cast<unsigned char>(bytes[i + 1]) : 0;
        unsigned c = (i + 2 < bytes.size()) ? static_cast<unsigned char>(bytes[i + 2]) : 0;
        unsigned n = (a << 16) | (b << 8) | c;
        out += T[(n >> 18) & 63];
        out += T[(n >> 12) & 63];
        out += (i + 1 < bytes.size()) ? T[(n >> 6) & 63] : '=';
        out += (i + 2 < bytes.size()) ? T[n & 63] : '=';
    }
    return out;
}

}  // namespace b64
