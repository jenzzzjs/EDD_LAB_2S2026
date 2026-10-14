#pragma once
// Servidor HTTP/1.1 minimo sobre Winsock: solo GET/POST/OPTIONS,
// cabeceras basicas y cuerpo por Content-Length. Suficiente para el API.

#include <functional>
#include <map>
#include <string>

struct HttpRequest {
    std::string method;
    std::string path;  // sin query
    std::map<std::string, std::string> query;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct HttpResponse {
    int status = 200;
    std::string contentType = "application/json";
    std::string body;
};

class HttpServer {
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;
    void route(const std::string& method, const std::string& path, Handler h);
    bool listen(int port);  // bloquea; retorna false si no puede bindear

private:
    std::map<std::string, Handler> routes_;  // "METODO /ruta"
    HttpResponse dispatch(const HttpRequest& req);
    static void handleClient(unsigned long long sockFd, HttpServer* self);
};
