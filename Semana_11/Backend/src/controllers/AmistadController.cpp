#include "AmistadController.h"

#include <algorithm>

#include "../models/Store.h"
#include "../utils/Json.h"

namespace AmistadController {

// Lista todos menos al que pregunta (excluir), y si viene q solo los que
// coincidan en nombre o correo. Cada uno dice si ya es amigo o no para que
// el frontend sepa que boton ponerle.
Result listarUsuarios(const std::string& excluir, const std::string& q) {
    std::string ex = toLowerStr(trimStr(excluir));
    std::string query = toLowerStr(trimStr(q));
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);

    std::vector<Usuario> todos;
    for (auto& kv : st.usuarios) {
        if (kv.first == ex) continue;
        if (!query.empty() && toLowerStr(kv.second.nombre).find(query) == std::string::npos &&
            kv.first.find(query) == std::string::npos)
            continue;
        todos.push_back(kv.second);
    }
    std::sort(todos.begin(), todos.end(),
              [](const Usuario& a, const Usuario& b) { return a.nombre < b.nombre; });

    std::string body = "{\"ok\":true,\"usuarios\":[";
    for (size_t i = 0; i < todos.size(); i++) {
        if (i > 0) body += ",";
        bool amigo = !ex.empty() && st.grafo.sonAmigos(ex, todos[i].correo);
        body += "{\"nombre\":" + json::str(todos[i].nombre) + ",\"correo\":" +
                json::str(todos[i].correo) + ",\"es_amigo\":" + (amigo ? "true" : "false") + "}";
    }
    body += "]}";
    return {200, body};
}

// Busca la solicitud de->para en la lista. Devuelve su posicion o -1.
static int buscarSolicitud(Store& st, const std::string& de, const std::string& para) {
    for (size_t i = 0; i < st.solicitudes.size(); i++)
        if (st.solicitudes[i].de == de && st.solicitudes[i].para == para) return (int)i;
    return -1;
}

// Enviar solicitud: ojo que todavia NO crea arista, solo deja el papelito
// en "pendiente". Se bloquea el auto-envio, los duplicados y el caso en que
// el otro ya me habia mandado una (ahi toca aceptar la de el, no mandar otra).
Result enviarSolicitud(const std::string& de, const std::string& para) {
    std::string d = toLowerStr(trimStr(de));
    std::string p = toLowerStr(trimStr(para));
    if (d.empty() || p.empty()) return {400, errJson("'de' y 'para' son obligatorios")};
    if (d == p) return {400, errJson("no puedes enviarte solicitud a ti mismo")};

    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    if (st.usuarios.find(d) == st.usuarios.end() || st.usuarios.find(p) == st.usuarios.end())
        return {404, errJson("usuario no encontrado")};
    if (st.grafo.sonAmigos(d, p)) return {409, errJson("ya son amigos")};

    int idx = buscarSolicitud(st, d, p);
    if (idx >= 0 && st.solicitudes[idx].estado == "pendiente")
        return {409, errJson("ya existe una solicitud pendiente")};
    int inv = buscarSolicitud(st, p, d);
    if (inv >= 0 && st.solicitudes[inv].estado == "pendiente")
        return {409, errJson("ese usuario ya te envio una solicitud; aceptala")};
    if (idx >= 0)
        st.solicitudes[idx].estado = "pendiente";  // reenviar tras un rechazo viejo
    else
        st.solicitudes.push_back(Solicitud{d, p, "pendiente"});
    return {201, "{\"ok\":true,\"mensaje\":\"solicitud enviada\"}"};
}

// Las que me mandaron y aun no respondo (para la campanita de pendientes).
Result pendientes(const std::string& correo) {
    std::string c = toLowerStr(trimStr(correo));
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    std::string body = "{\"ok\":true,\"pendientes\":[";
    bool primero = true;
    for (size_t i = 0; i < st.solicitudes.size(); i++) {
        const Solicitud& s = st.solicitudes[i];
        if (s.para == c && s.estado == "pendiente") {
            if (!primero) body += ",";
            primero = false;
            std::string nombre = s.de;
            auto it = st.usuarios.find(s.de);
            if (it != st.usuarios.end()) nombre = it->second.nombre;
            body += "{\"de\":" + json::str(s.de) + ",\"nombre\":" + json::str(nombre) +
                    ",\"estado\":\"pendiente\"}";
        }
    }
    body += "]}";
    return {200, body};
}

// Las que yo mande y siguen sin respuesta.
Result enviadas(const std::string& correo) {
    std::string c = toLowerStr(trimStr(correo));
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    std::string body = "{\"ok\":true,\"enviadas\":[";
    bool primero = true;
    for (size_t i = 0; i < st.solicitudes.size(); i++) {
        const Solicitud& s = st.solicitudes[i];
        if (s.de == c && s.estado == "pendiente") {
            if (!primero) body += ",";
            primero = false;
            body += "{\"de\":" + json::str(s.de) + ",\"para\":" + json::str(s.para) +
                    ",\"estado\":\"pendiente\"}";
        }
    }
    body += "]}";
    return {200, body};
}

// Aceptar: la solicitud pasa a aceptada Y aqui si nace la arista en el grafo
// (agregarArista la mete en los dos sentidos porque no es dirigido).
Result aceptar(const std::string& de, const std::string& para) {
    std::string d = toLowerStr(trimStr(de));
    std::string p = toLowerStr(trimStr(para));
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    int idx = buscarSolicitud(st, d, p);
    if (idx < 0 || st.solicitudes[idx].estado != "pendiente")
        return {404, errJson("solicitud pendiente no encontrada")};
    st.solicitudes[idx].estado = "aceptada";
    st.grafo.agregarArista(d, p);
    return {200, "{\"ok\":true,\"mensaje\":\"amistad aceptada\"}"};
}

// Rechazar: solo cambia el estado, el grafo ni se entera (no hay arista).
Result rechazar(const std::string& de, const std::string& para) {
    std::string d = toLowerStr(trimStr(de));
    std::string p = toLowerStr(trimStr(para));
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    int idx = buscarSolicitud(st, d, p);
    if (idx < 0 || st.solicitudes[idx].estado != "pendiente")
        return {404, errJson("solicitud pendiente no encontrada")};
    st.solicitudes[idx].estado = "rechazada";
    return {200, "{\"ok\":true,\"mensaje\":\"solicitud rechazada\"}"};
}

// Los amigos no son mas que los vecinos del vertice en el grafo.
Result listarAmigos(const std::string& correo) {
    std::string c = toLowerStr(trimStr(correo));
    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    if (st.usuarios.find(c) == st.usuarios.end())
        return {404, errJson("usuario no encontrado")};
    std::vector<Vertice> amigos = st.grafo.obtenerAmigos(c);
    std::string body = "{\"ok\":true,\"amigos\":[";
    for (size_t i = 0; i < amigos.size(); i++) {
        if (i > 0) body += ",";
        body += "{\"nombre\":" + json::str(amigos[i].nombre) + ",\"correo\":" +
                json::str(amigos[i].correo) + "}";
    }
    body += "]}";
    return {200, body};
}

}  // namespace AmistadController
