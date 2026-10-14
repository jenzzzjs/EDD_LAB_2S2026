#pragma once
// Servidor HTTP/1.1 minimo sobre Winsock: solo GET/POST/OPTIONS,
// cabeceras basicas y cuerpo por Content-Length. Suficiente para el API.
// Tablas hechas a mano con Lista (sin std::map): rutas, cabeceras y query.

#include <functional>
#include <string>

#include "../models/Lista.h"

struct HttpRequest {
    std::string method;
    std::string path;  // sin query
    Lista<Par> query;
    Lista<Par> headers;
    std::string body;
};

struct HttpResponse {
    int status = 200;
    std::string contentType = "application/json";
    std::string body;
};

// Una ruta registrada: "METODO /ruta" -> su funcion.
struct Ruta {
    std::string clave;
    std::function<HttpResponse(const HttpRequest&)> handler;
};

class HttpServer {
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;
    void route(const std::string& method, const std::string& path, Handler h);
    bool listen(int port);  // bloquea; retorna false si no puede bindear

private:
    Lista<Ruta> routes_;
    HttpResponse dispatch(const HttpRequest& req);
    static void handleClient(unsigned long long sockFd, HttpServer* self);
};
