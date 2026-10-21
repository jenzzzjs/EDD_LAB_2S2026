#pragma once
// Servidor HTTP/1.1 minimo sobre Winsock: solo GET/POST/OPTIONS,
// cabeceras basicas y cuerpo por Content-Length. Suficiente para el API.
// Tablas hechas a mano con Lista, sin std::map: rutas, cabeceras y query.
//
// En la Semana 12 el mismo servidor tambien acepta WebSocket. La conexion
// empieza como una peticion HTTP normal y, si el cliente pide el cambio de
// protocolo, el servidor responde 101 y entrega el socket a un handler de
// tramas. Por eso HTTP y WebSocket comparten el mismo puerto.

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
    // El handler de WebSocket recibe el socket crudo despues del handshake. En
    // ese momento el socket ya no transporta peticiones HTTP sino tramas
    // WebSocket, por eso el handler se queda leyendo hasta que el cliente se
    // desconecta o ocurre un error.
    using WebSocketHandler = std::function<void(unsigned long long, const HttpRequest&)>;

    void route(const std::string& method, const std::string& path, Handler h);
    void websocket(const std::string& path, WebSocketHandler h);
    bool listen(int port);  // bloquea; retorna false si no puede bindear

private:
    Lista<Ruta> routes_;
    std::string wsPath_;
    WebSocketHandler wsHandler_;
    HttpResponse dispatch(const HttpRequest& req);
    static void handleClient(unsigned long long sockFd, HttpServer* self);
};
