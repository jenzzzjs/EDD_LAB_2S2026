#include "Json.h"

#include <cstdio>

namespace json {

std::string escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else {
                    out += c;
                }
        }
    }
    return out;
}

std::string str(const std::string& s) { return "\"" + escape(s) + "\""; }

static void skipWs(const std::string& s, size_t& i) {
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')) i++;
}

// Lee un string JSON desde s[i] (s[i] debe ser '"'), deja i despues del cierre.
static bool readQuoted(const std::string& s, size_t& i, std::string& out) {
    if (i >= s.size() || s[i] != '"') return false;
    i++;
    out.clear();
    while (i < s.size()) {
        char c = s[i];
        if (c == '\\' && i + 1 < s.size()) {
            char e = s[i + 1];
            switch (e) {
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                default: out += e; break;
            }
            i += 2;
        } else if (c == '"') {
            i++;
            return true;
        } else {
            out += c;
            i++;
        }
    }
    return false;
}

std::string getString(const std::string& body, const std::string& key) {
    // Busca "key" a mano (sin regex): recorre los pares del objeto.
    size_t i = 0;
    skipWs(body, i);
    if (i < body.size() && body[i] == '{') i++;
    while (i < body.size()) {
        skipWs(body, i);
        if (i >= body.size() || body[i] == '}') break;
        std::string k;
        if (!readQuoted(body, i, k)) break;
        skipWs(body, i);
        if (i >= body.size() || body[i] != ':') break;
        i++;
        skipWs(body, i);
        std::string v;
        if (i < body.size() && body[i] == '"') {
            readQuoted(body, i, v);
        } else {
            // numero/bool/null: leer hasta , o }
            size_t j = i;
            while (j < body.size() && body[j] != ',' && body[j] != '}') j++;
            v = body.substr(i, j - i);
            i = j;
        }
        if (k == key) return v;
        skipWs(body, i);
        if (i < body.size() && body[i] == ',') i++;
    }
    return "";
}

static int hexVal(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

std::string urlDecode(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '%' && i + 2 < s.size()) {
            int a = hexVal(s[i + 1]), b = hexVal(s[i + 2]);
            if (a >= 0 && b >= 0) {
                out += static_cast<char>(a * 16 + b);
                i += 2;
                continue;
            }
        }
        out += (s[i] == '+') ? ' ' : s[i];
    }
    return out;
}

std::map<std::string, std::string> parseQuery(const std::string& qs) {
    std::map<std::string, std::string> m;
    size_t i = 0;
    while (i <= qs.size()) {
        size_t j = qs.find('&', i);
        if (j == std::string::npos) j = qs.size();
        std::string par = qs.substr(i, j - i);
        size_t eq = par.find('=');
        if (eq != std::string::npos)
            m[urlDecode(par.substr(0, eq))] = urlDecode(par.substr(eq + 1));
        else if (!par.empty())
            m[urlDecode(par)] = "";
        i = j + 1;
    }
    return m;
}

}  // namespace json
