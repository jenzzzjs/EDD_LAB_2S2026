#include "HttpServer.h"

#include <cstdio>
#include <thread>

#include "../utils/Json.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using sock_t = SOCKET;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using sock_t = int;
#endif

static void closeSock(sock_t s) {
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

static bool sendAll(sock_t s, const char* data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
#ifdef _WIN32
        int n = send(s, data + sent, (int)(len - sent), 0);
#else
        ssize_t n = send(s, data + sent, len - sent, 0);
#endif
        if (n <= 0) return false;
        sent += (size_t)n;
    }
    return true;
}

void HttpServer::route(const std::string& method, const std::string& path, Handler h) {
    // La tablita de rutas: "GET /api/amigos" -> su funcion. Asi de simple.
    routes_[method + " " + path] = h;
}

static std::string lowerHeader(std::string s) {
    for (size_t i = 0; i < s.size(); i++)
        s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}

static std::string trimSv(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

HttpResponse HttpServer::dispatch(const HttpRequest& req) {
    // Busca "METODO + ruta" en la tabla; si no existe, 404 de una vez.
    auto it = routes_.find(req.method + " " + req.path);
    if (it == routes_.end()) {
        HttpResponse r;
        r.status = 404;
        r.body = "{\"ok\":false,\"error\":\"ruta no encontrada\"}";
        return r;
    }
    return it->second(req);
}

void HttpServer::handleClient(unsigned long long sockFd, HttpServer* self) {
    sock_t cs = (sock_t)sockFd;
    std::string data;
    char buf[4096];
    // Leer hasta fin de cabeceras.
    while (data.find("\r\n\r\n") == std::string::npos && data.size() < 65536) {
#ifdef _WIN32
        int n = recv(cs, buf, sizeof(buf), 0);
#else
        ssize_t n = recv(cs, buf, sizeof(buf), 0);
#endif
        if (n <= 0) break;
        data.append(buf, (size_t)n);
    }
    size_t hEnd = data.find("\r\n\r\n");
    HttpResponse res;
    if (hEnd == std::string::npos) {
        res.status = 400;
        res.body = "{\"ok\":false,\"error\":\"peticion invalida\"}";
    } else {
        // Linea de peticion.
        size_t eol = data.find("\r\n");
        std::string reqLine = data.substr(0, eol);
        size_t sp1 = reqLine.find(' ');
        size_t sp2 = reqLine.find(' ', sp1 + 1);
        HttpRequest req;
        if (sp1 == std::string::npos || sp2 == std::string::npos) {
            res.status = 400;
            res.body = "{\"ok\":false,\"error\":\"peticion invalida\"}";
        } else {
            req.method = reqLine.substr(0, sp1);
            std::string target = reqLine.substr(sp1 + 1, sp2 - sp1 - 1);
            // Cabeceras.
            size_t pos = eol + 2;
            while (pos < hEnd) {
                size_t nl = data.find("\r\n", pos);
                std::string line = data.substr(pos, nl - pos);
                size_t c = line.find(':');
                if (c != std::string::npos)
                    req.headers[lowerHeader(trimSv(line.substr(0, c)))] =
                        trimSv(line.substr(c + 1));
                pos = nl + 2;
            }
            // Cuerpo segun Content-Length.
            size_t contentLen = 0;
            auto cl = req.headers.find("content-length");
            if (cl != req.headers.end()) contentLen = (size_t)strtoul(cl->second.c_str(), nullptr, 10);
            std::string body = data.substr(hEnd + 4);
            while (body.size() < contentLen && body.size() < 10 * 1024 * 1024) {
#ifdef _WIN32
                int n = recv(cs, buf, sizeof(buf), 0);
#else
                ssize_t n = recv(cs, buf, sizeof(buf), 0);
#endif
                if (n <= 0) break;
                body.append(buf, (size_t)n);
            }
            req.body = body;
            // Path + query.
            size_t q = target.find('?');
            if (q == std::string::npos) {
                req.path = target;
            } else {
                req.path = target.substr(0, q);
                req.query = json::parseQuery(target.substr(q + 1));
            }
            if (req.path.empty()) req.path = "/";

            // Preflight CORS: el navegador manda OPTIONS antes del POST/GET real
            // cuando viene de otro puerto (el frontend en :5173). Se responde
            // 204 vacio y las cabeceras de abajo hacen el resto.
            if (req.method == "OPTIONS") {
                res.status = 204;
                res.body = "";
            } else {
                res = self->dispatch(req);
            }
        }
    }

    const char* reason = "OK";
    switch (res.status) {
        case 200: reason = "OK"; break;
        case 201: reason = "Created"; break;
        case 204: reason = "No Content"; break;
        case 400: reason = "Bad Request"; break;
        case 401: reason = "Unauthorized"; break;
        case 404: reason = "Not Found"; break;
        case 409: reason = "Conflict"; break;
        case 500: reason = "Internal Server Error"; break;
        default: reason = "OK"; break;
    }
    std::string head = "HTTP/1.1 " + std::to_string(res.status) + " " + reason + "\r\n";
    head += "Content-Type: " + res.contentType + "\r\n";
    head += "Content-Length: " + std::to_string(res.body.size()) + "\r\n";
    // Sin estas 3 lineas el navegador bloquea al frontend por CORS, porque
    // el API (:5000) y la pagina (:5173) son "origenes" distintos.
    head += "Access-Control-Allow-Origin: *\r\n";
    head += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    head += "Access-Control-Allow-Headers: Content-Type\r\n";
    head += "Connection: close\r\n\r\n";
    sendAll(cs, head.c_str(), head.size());
    if (!res.body.empty()) sendAll(cs, res.body.c_str(), res.body.size());
    closeSock(cs);
}

bool HttpServer::listen(int port) {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;
#endif
    sock_t srv = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
    if (srv == INVALID_SOCKET) return false;
#else
    if (srv < 0) return false;
#endif
    int opt = 1;
#ifdef _WIN32
    // En Windows SO_REUSEADDR permitiria que DOS instancias escuchen el
    // mismo puerto (cada una con su propia memoria). Se usa EXCLUSIVE para
    // que la segunda instancia falle en bind() en vez de duplicar el servicio.
    setsockopt(srv, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char*)&opt, sizeof(opt));
#else
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#endif

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((uint16_t)port);
    if (bind(srv, (sockaddr*)&addr, sizeof(addr)) != 0) {
        closeSock(srv);
        return false;
    }
    if (::listen(srv, 64) != 0) {
        closeSock(srv);
        return false;
    }
    printf("Corriendo en el puerto %d - http://localhost:%d\n", port, port);
    fflush(stdout);
    while (true) {
        sock_t cs = accept(srv, nullptr, nullptr);
#ifdef _WIN32
        if (cs == INVALID_SOCKET) break;
#else
        if (cs < 0) break;
#endif
        std::thread(handleClient, (unsigned long long)cs, this).detach();
    }
    closeSock(srv);
#ifdef _WIN32
    WSACleanup();
#endif
    return true;
}
