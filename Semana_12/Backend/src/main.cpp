// Backend Semana 12 - Red social + chat en C++.
//
// La Semana 11 dejo el modelo de usuarios y el grafo de amistades. En la
// Semana 12 agregamos dos cosas. La primera es LZW, que comprime cada
// conversacion antes de guardarla en lzw/*.lzw. La segunda es WebSocket, que
// mantiene una conexion abierta entre el navegador y el servidor para enviar
// mensajes en tiempo real por el mismo puerto HTTP.
//
// Modelo:      models/Store, usuarios y solicitudes en memoria
//              models/Grafo, grafo no dirigido con lista de adyacencia
//              models/ChatStore, conversaciones persistidas en lzw/*.lzw
// Vista:       JSON y PNG generado con Graphviz
// Controlador: controllers/*  Rutas: routes/*  HTTP: http/HttpServer
//              WebSocket nativo en http/WebSocket, mismo puerto
//
// Compilar con MSYS2 o MinGW:
//   g++ -std=c++17 -O2 -Isrc src/main.cpp src/utils/*.cpp src/models/*.cpp
//       src/controllers/*.cpp src/http/*.cpp src/routes/*.cpp -lws2_32 -o backend.exe
// Ejecutar:
//   ./backend.exe 5000
//
// Los usuarios y amistades siguen en memoria, pero cada conversacion se
// guarda comprimida con LZW en la carpeta lzw y sobrevive al reinicio.

#include <cstdio>
#include <cstdlib>

#include "http/HttpServer.h"
#include "models/ChatStore.h"
#include "routes/Routes.h"

int main(int argc, char** argv) {
    int port = 5000;
    if (argc > 1) port = atoi(argv[1]);
    if (port <= 0) port = 5000;

    // Creamos la carpeta lzw al arrancar, aunque todavia no haya mensajes.
    ChatStore::get();

    HttpServer server;
    Routes::registrar(server);
    printf("Chat WebSocket en ws://localhost:%d/ws\n", port);
    printf("Archivos LZW en %s/\n", ChatStore::get().carpeta().c_str());
    // El servidor atiende HTTP y WebSocket en el mismo puerto. Cada conexion
    // WebSocket ocupa un hilo mientras la pestana siga abierta.
    if (!server.listen(port)) {
        printf("ERROR: no se pudo escuchar en el puerto %d (ya hay otra instancia?)\n", port);
        return 1;
    }
    return 0;
}
