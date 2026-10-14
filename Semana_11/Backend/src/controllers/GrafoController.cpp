#include "GrafoController.h"

#include "../models/Store.h"
#include "../utils/Base64.h"
#include "../utils/Graphviz.h"
#include "../utils/Json.h"

namespace GrafoController {

// Un vertice en JSON es solo nombre + correo.
static std::string verticeJson(const Vertice& v) {
    return "{\"nombre\":" + json::str(v.nombre) + ",\"correo\":" + json::str(v.correo) + "}";
}

// La red de un usuario: se le pide al grafo su red de 2 niveles, se genera
// el DOT con los colores pedidos (yo verde, amigos azul, en comun gris) y
// se renderiza a PNG. El PNG viaja en base64 dentro del JSON para que el
// frontend lo muestre con un <img> sin pedir otro endpoint.
Result redDeUsuario(const std::string& correo) {
    std::string c = toLowerStr(trimStr(correo));
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    if (st.usuarios.find(c) == st.usuarios.end())
        return {404, errJson("usuario no encontrado")};

    RedUsuario red = st.grafo.redDeUsuario(c);
    std::string dot = st.grafo.toDotRed(c);

    std::string body = "{\"ok\":true,";
    body += "\"yo\":" + verticeJson(red.yo) + ",\"amigos\":[";
    for (size_t i = 0; i < red.amigos.size(); i++) {
        if (i > 0) body += ",";
        body += verticeJson(red.amigos[i]);
    }
    body += "],\"amigos_en_comun\":[";
    for (size_t i = 0; i < red.enComun.size(); i++) {
        if (i > 0) body += ",";
        body += "{\"nombre\":" + json::str(red.enComun[i].nombre) + ",\"correo\":" +
                json::str(red.enComun[i].correo) + ",\"conecta_con\":[";
        for (size_t j = 0; j < red.enComun[i].conectaCon.size(); j++) {
            if (j > 0) body += ",";
            body += json::str(red.enComun[i].conectaCon[j]);
        }
        body += "]}";
    }
    body += "],\"aristas\":[";
    for (size_t i = 0; i < red.aristas.size(); i++) {
        if (i > 0) body += ",";
        body += "[" + json::str(red.aristas[i].first) + "," + json::str(red.aristas[i].second) + "]";
    }
    body += "],\"dot\":" + json::str(dot) + ",";
    body += "\"colores\":{\"yo\":\"verde #22c55e\",\"amigos\":\"azul #2563eb\","
            "\"en_comun\":\"gris #9ca3af\"}";

    std::string png = graphviz::renderDotAPng(dot);
    if (!png.empty()) {
        body += ",\"imagen_base64\":" + json::str(b64::encode(png));
    } else {
        // Si el `dot` no esta instalado igual se devuelve todo lo demas y
        // se avisa, para que el frontend lo muestre como texto.
        body += ",\"imagen_base64\":null,\"advertencia\":" +
                json::str(graphviz::ultimoError().empty() ? "Graphviz no disponible"
                                                          : graphviz::ultimoError());
    }
    body += "}";
    return {200, body};
}

// El grafo completo (todos los usuarios y todas las amistades) con su DOT
// y su PNG, por si se quiere ver la red entera y no solo la de uno.
Result grafoCompleto() {
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    std::string dot = st.grafo.toDotCompleto();
    std::vector<Vertice> vs = st.grafo.todosLosVertices();
    std::vector<std::pair<std::string, std::string>> ar = st.grafo.todasLasAristas();

    std::string body = "{\"ok\":true,\"vertices\":[";
    for (size_t i = 0; i < vs.size(); i++) {
        if (i > 0) body += ",";
        body += verticeJson(vs[i]);
    }
    body += "],\"aristas\":[";
    for (size_t i = 0; i < ar.size(); i++) {
        if (i > 0) body += ",";
        body += "[" + json::str(ar[i].first) + "," + json::str(ar[i].second) + "]";
    }
    body += "],\"dot\":" + json::str(dot);
    std::string png = graphviz::renderDotAPng(dot);
    if (!png.empty()) {
        body += ",\"imagen_base64\":" + json::str(b64::encode(png));
    } else {
        body += ",\"imagen_base64\":null,\"advertencia\":" + json::str("Graphviz no disponible");
    }
    body += "}";
    return {200, body};
}

// El PNG solito, para usarlo directo como src de un <img> sin pasar por JSON.
Result redPng(const std::string& correo) {
    std::string c = toLowerStr(trimStr(correo));
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    if (st.usuarios.find(c) == st.usuarios.end())
        return {404, errJson("usuario no encontrado")};
    std::string dot = st.grafo.toDotRed(c);
    std::string png = graphviz::renderDotAPng(dot);
    if (png.empty()) return {500, errJson("no se pudo generar el PNG con Graphviz")};
    Result r;
    r.status = 200;
    r.body = png;
    r.contentType = "image/png";
    return r;
}

// Dice si el `dot` esta instalado (el frontend lo puede consultar).
Result estado() {
    Result r;
    r.body = std::string("{\"ok\":true,\"graphviz\":") +
             (graphviz::disponible() ? "true" : "false") + "}";
    return r;
}

}  // namespace GrafoController
