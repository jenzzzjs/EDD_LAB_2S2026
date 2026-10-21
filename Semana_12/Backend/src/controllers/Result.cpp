#include "Result.h"

#include <cctype>

#include "../utils/Json.h"

std::string toLowerStr(const std::string& s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); i++)
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    return out;
}

std::string trimStr(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && (s[a] == ' ' || s[a] == '\t' || s[a] == '\n' || s[a] == '\r')) a++;
    size_t b = s.size();
    while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\n' || s[b - 1] == '\r')) b--;
    return s.substr(a, b - a);
}

std::string errJson(const std::string& msg) {
    return "{\"ok\":false,\"error\":" + json::str(msg) + "}";
}
