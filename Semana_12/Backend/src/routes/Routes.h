#pragma once
// Rutas del API: conectan HTTP <-> Controladores (MVC).

#include <string>

class HttpServer;

namespace Routes {

void registrar(HttpServer& server);

}  // namespace Routes
