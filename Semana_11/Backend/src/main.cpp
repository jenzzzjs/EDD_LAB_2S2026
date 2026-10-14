// Backend Semana 11 - Red social (C++ MVC, sin frameworks).
//
//   Modelo:      models/Store (usuarios + solicitudes, SOLO en memoria)
//                models/Grafo (grafo NO dirigido a mano, lista de adyacencia)
//   Vista:       JSON (+ PNG via Graphviz) que consume el frontend
//   Controlador: controllers/*  Rutas: routes/*  HTTP: http/HttpServer
//
// Compilar (MSYS2/MinGW):
//   g++ -std=c++17 -O2 -Isrc src/main.cpp src/utils/*.cpp src/models/*.cpp
//       src/controllers/*.cpp src/http/*.cpp src/routes/*.cpp -lws2_32 -o server.exe
// Ejecutar:
//   ./backend.exe 5000
//
// Nota: los datos viven solo en memoria; al apagar el servidor se pierden.
// Por eso no hay data.json ni archivos: todo esta en el Store mientras corre.

#include <cstdio>
#include <cstdlib>

#include "http/HttpServer.h"
#include "routes/Routes.h"

int main(int argc, char** argv) {
    int port = 5000;
    if (argc > 1) port = atoi(argv[1]);
    if (port <= 0) port = 5000;

    HttpServer server;
    Routes::registrar(server);
    // listen() se queda aqui atendiendo para siempre (un hilo por cliente).
    if (!server.listen(port)) {
        printf("ERROR: no se pudo escuchar en el puerto %d (ya hay otra instancia?)\n", port);
        return 1;
    }
    return 0;
}
