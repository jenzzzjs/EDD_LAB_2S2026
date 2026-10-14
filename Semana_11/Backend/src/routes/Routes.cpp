#include "Routes.h"

#include "../controllers/AmistadController.h"
#include "../controllers/AuthController.h"
#include "../controllers/GrafoController.h"
#include "../http/HttpServer.h"
#include "../models/Store.h"
#include "../utils/Json.h"

namespace Routes {

// Saca un query param (?correo=...) ya con URL-decode aplicado.
static std::string q(const HttpRequest& req, const std::string& key) {
    auto it = req.query.find(key);
    return it == req.query.end() ? "" : it->second;
}

// Aqui se conectan las URLs con los controladores: cada ruta saca sus
// parametros (del JSON o del query) y le pasa la chamba al controlador.
// La "vista" es el JSON que se devuelve, que es lo que el frontend pinta.
void registrar(HttpServer& server) {
    server.route("GET", "/api/health", [](const HttpRequest&) {
        return HttpResponse{200, "application/json",
                            "{\"ok\":true,\"servicio\":\"backend-semana11-mvc-cpp\"}"};
    });

// Reset solo para el lab: vacia la memoria sin tener que reiniciar.
    server.route("POST", "/api/reset", [](const HttpRequest&) {
        Store& st = Store::get();
        std::lock_guard<std::mutex> lock(st.mtx);
        st.reset();
        return HttpResponse{200, "application/json", "{\"ok\":true}"};
    });

    server.route("POST", "/api/auth/registro", [](const HttpRequest& req) {
        Result r = AuthController::registro(json::getString(req.body, "nombre"),
                                            json::getString(req.body, "correo"),
                                            json::getString(req.body, "password"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("POST", "/api/auth/login", [](const HttpRequest& req) {
        Result r = AuthController::login(json::getString(req.body, "correo"),
                                         json::getString(req.body, "password"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("GET", "/api/usuarios", [](const HttpRequest& req) {
        Result r = AmistadController::listarUsuarios(q(req, "excluir"), q(req, "q"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("GET", "/api/amigos", [](const HttpRequest& req) {
        Result r = AmistadController::listarAmigos(q(req, "correo"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("POST", "/api/solicitudes", [](const HttpRequest& req) {
        Result r = AmistadController::enviarSolicitud(json::getString(req.body, "de"),
                                                      json::getString(req.body, "para"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("GET", "/api/solicitudes/pendientes", [](const HttpRequest& req) {
        Result r = AmistadController::pendientes(q(req, "correo"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("GET", "/api/solicitudes/enviadas", [](const HttpRequest& req) {
        Result r = AmistadController::enviadas(q(req, "correo"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("POST", "/api/solicitudes/aceptar", [](const HttpRequest& req) {
        Result r = AmistadController::aceptar(json::getString(req.body, "de"),
                                              json::getString(req.body, "para"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("POST", "/api/solicitudes/rechazar", [](const HttpRequest& req) {
        Result r = AmistadController::rechazar(json::getString(req.body, "de"),
                                               json::getString(req.body, "para"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("GET", "/api/grafo", [](const HttpRequest&) {
        Result r = GrafoController::grafoCompleto();
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("GET", "/api/grafo/red", [](const HttpRequest& req) {
        Result r = GrafoController::redDeUsuario(q(req, "correo"));
        return HttpResponse{r.status, "application/json", r.body};
    });

    server.route("GET", "/api/grafo/red.png", [](const HttpRequest& req) {
        Result r = GrafoController::redPng(q(req, "correo"));
        return HttpResponse{r.status, r.contentType, r.body};
    });

    server.route("GET", "/api/grafo/estado", [](const HttpRequest&) {
        Result r = GrafoController::estado();
        return HttpResponse{r.status, "application/json", r.body};
    });
}

}  // namespace Routes
