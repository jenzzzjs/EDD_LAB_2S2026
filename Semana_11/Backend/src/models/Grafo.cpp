#include "Grafo.h"

#include <algorithm>

// --- Ayudas internas ---

// Recorre el vector comparando uno por uno. Asi de simple, sin set ni find,
// porque la idea es que la estructura se entienda a puro ciclo.
bool GrafoNoDirigido::contiene(const std::vector<std::string>& v, const std::string& x) {
    for (size_t i = 0; i < v.size(); i++)
        if (v[i] == x) return true;
    return false;
}

// Si el texto lleva comilla o backslash hay que escaparlos, si no el DOT
// sale mal formado y el `dot` se queja.
std::string GrafoNoDirigido::esc(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"')
            out += "\\\"";
        else if (c == '\\')
            out += "\\\\";
        else
            out += c;
    }
    return out;
}

// Hash "a manita" (base 31, como el de la tabla hash de la semana 9) para
// inventar un ID corto tipo n123456 a partir del correo.
std::string GrafoNoDirigido::idNodo(const std::string& correo) {
    unsigned h = 0;
    for (char c : correo) h = h * 31 + static_cast<unsigned char>(c);
    return "n" + std::to_string(h);
}

// --- Vertices ---

bool GrafoNoDirigido::existeVertice(const std::string& correo) const {
    return vertices_.find(correo) != vertices_.end();
}

// Meter un vertice es crear su ficha y dejarle su lista de vecinos vacia,
// porque recien registrado nadie es amigo de nadie todavia.
bool GrafoNoDirigido::agregarVertice(const std::string& nombre, const std::string& correo) {
    if (existeVertice(correo)) return false;
    vertices_[correo] = Vertice{nombre, correo};
    ady_[correo] = std::vector<std::string>();
    return true;
}

// --- Aristas (la amistad) ---

bool GrafoNoDirigido::sonAmigos(const std::string& a, const std::string& b) const {
    auto it = ady_.find(a);
    if (it == ady_.end()) return false;
    return contiene(it->second, b);
}

// Aceptar una solicitud termina aqui: se agrega la arista en AMBOS sentidos
// porque la amistad no tiene direccion (si Ana es amiga de Bruno, Bruno es
// amigo de Ana). No se permiten bucles (a--a) ni aristas repetidas.
bool GrafoNoDirigido::agregarArista(const std::string& a, const std::string& b) {
    if (a == b) return false;
    if (!existeVertice(a) || !existeVertice(b)) return false;
    if (contiene(ady_[a], b)) return false;
    ady_[a].push_back(b);
    ady_[b].push_back(a);
    return true;
}

// --- Consultas ---

// Los amigos directos no son mas que la lista de vecinos del vertice.
// Se ordenan por nombre para que el frontend los muestre siempre igual.
std::vector<Vertice> GrafoNoDirigido::obtenerAmigos(const std::string& correo) const {
    std::vector<Vertice> out;
    auto it = ady_.find(correo);
    if (it == ady_.end()) return out;
    for (size_t i = 0; i < it->second.size(); i++) {
        auto vit = vertices_.find(it->second[i]);
        if (vit != vertices_.end()) out.push_back(vit->second);
    }
    std::sort(out.begin(), out.end(),
              [](const Vertice& x, const Vertice& y) { return x.nombre < y.nombre; });
    return out;
}

// Amigos en comun = los que aparecen en las dos listas de vecinos.
// Se hace con doble ciclo (para cada vecino de a, se busca en los de b)
// y se evita meter duplicados.
std::vector<Vertice> GrafoNoDirigido::amigosEnComun(const std::string& a,
                                                   const std::string& b) const {
    std::vector<Vertice> out;
    auto ita = ady_.find(a);
    auto itb = ady_.find(b);
    if (ita == ady_.end() || itb == ady_.end()) return out;
    for (size_t i = 0; i < ita->second.size(); i++) {
        for (size_t j = 0; j < itb->second.size(); j++) {
            if (ita->second[i] == itb->second[j]) {
                auto vit = vertices_.find(ita->second[i]);
                if (vit != vertices_.end()) {
                    bool dup = false;
                    for (size_t k = 0; k < out.size(); k++)
                        if (out[k].correo == vit->second.correo) dup = true;
                    if (!dup) out.push_back(vit->second);
                }
            }
        }
    }
    std::sort(out.begin(), out.end(),
              [](const Vertice& x, const Vertice& y) { return x.nombre < y.nombre; });
    return out;
}

// Revisa si la arista a--b ya esta guardada, sin importar el orden en que
// se guardo (sirve porque el grafo no es dirigido).
static bool mismaArista(const std::pair<std::string, std::string>& e, const std::string& a,
                        const std::string& b) {
    return (e.first == a && e.second == b) || (e.first == b && e.second == a);
}

// Arma la red de 2 niveles: yo, mis amigos, y los amigos de mis amigos que
// todavia no son mis amigos (esos son los "en comun"). De paso junta todas
// las aristas que hay que dibujar: yo--amigo y amigo--en_comun. Si dos de
// mis amigos son amigos entre ellos, esa arista tambien se dibuja.
RedUsuario GrafoNoDirigido::redDeUsuario(const std::string& correo) const {
    RedUsuario red;
    auto it = vertices_.find(correo);
    if (it == vertices_.end()) return red;
    red.existe = true;
    red.yo = it->second;
    red.amigos = obtenerAmigos(correo);

    std::vector<std::string> correosAmigos;
    for (size_t i = 0; i < red.amigos.size(); i++) {
        correosAmigos.push_back(red.amigos[i].correo);
        red.aristas.push_back({correo, red.amigos[i].correo});
    }

    // Segundo nivel: se visitan los vecinos de cada amigo, uno por uno.
    for (size_t i = 0; i < red.amigos.size(); i++) {
        const std::string& ca = red.amigos[i].correo;
        auto vec = ady_.find(ca);
        if (vec == ady_.end()) continue;
        for (size_t j = 0; j < vec->second.size(); j++) {
            const std::string& v = vec->second[j];
            if (v == correo) continue;  // soy yo, no me dibujo dos veces
            if (contiene(correosAmigos, v)) {
                // Es amigo directo tambien: solo falta la arista amigo--amigo.
                bool ya = false;
                for (size_t k = 0; k < red.aristas.size(); k++)
                    if (mismaArista(red.aristas[k], ca, v)) ya = true;
                if (!ya) red.aristas.push_back({ca, v});
                continue;
            }
            auto vit = vertices_.find(v);
            if (vit == vertices_.end()) continue;
            // Si es la primera vez que lo vemos, se crea su ficha de "en comun".
            // Si ya estaba, solo se anota con que otro amigo conecta.
            int idx = -1;
            for (size_t k = 0; k < red.enComun.size(); k++)
                if (red.enComun[k].correo == v) idx = (int)k;
            if (idx < 0) {
                EnComun ec;
                ec.nombre = vit->second.nombre;
                ec.correo = v;
                ec.conectaCon.push_back(ca);
                red.enComun.push_back(ec);
            } else if (!contiene(red.enComun[idx].conectaCon, ca)) {
                red.enComun[idx].conectaCon.push_back(ca);
            }
            bool ya = false;
            for (size_t k = 0; k < red.aristas.size(); k++)
                if (mismaArista(red.aristas[k], ca, v)) ya = true;
            if (!ya) red.aristas.push_back({ca, v});
        }
    }
    std::sort(red.enComun.begin(), red.enComun.end(),
              [](const EnComun& x, const EnComun& y) { return x.nombre < y.nombre; });
    return red;
}

// Recorre toda la adyacencia y junta cada arista una sola vez (si ya salio
// a--b, no se vuelve a meter b--a).
std::vector<std::pair<std::string, std::string>> GrafoNoDirigido::todasLasAristas() const {
    std::vector<std::pair<std::string, std::string>> out;
    for (auto it = ady_.begin(); it != ady_.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); i++) {
            const std::string& a = it->first;
            const std::string& b = it->second[i];
            bool dup = false;
            for (size_t k = 0; k < out.size(); k++)
                if (mismaArista(out[k], a, b)) dup = true;
            if (!dup) out.push_back({a, b});
        }
    }
    return out;
}

std::vector<Vertice> GrafoNoDirigido::todosLosVertices() const {
    std::vector<Vertice> out;
    for (auto it = vertices_.begin(); it != vertices_.end(); ++it) out.push_back(it->second);
    return out;
}

// --- DOT para Graphviz (igual que en las semanas pasadas) ---

// El DOT se escribe a mano, linea por linea: primero los nodos con su color
// segun el grupo (yo verde, amigos azul, en comun gris) y despues las
// aristas con -- porque el grafo NO es dirigido (con -> seria dirigido).
std::string GrafoNoDirigido::toDotRed(const std::string& correo) const {
    std::string d = "graph RedDeAmigos {\n";
    d += "  layout=neato; overlap=false; splines=true;\n";
    d += "  node [style=filled fontname=\"Helvetica\" fontsize=11];\n";
    d += "  edge [color=\"#64748b\"];\n";
    RedUsuario red = redDeUsuario(correo);
    if (!red.existe) {
        d += "}\n";
        return d;
    }
    std::map<std::string, std::string> ids;
    std::string idYo = idNodo(red.yo.correo);
    ids[red.yo.correo] = idYo;
    d += "  " + idYo + " [label=\"" + esc(red.yo.nombre) +
         "\" fillcolor=\"#22c55e\" fontcolor=\"white\" shape=circle];\n";
    for (size_t i = 0; i < red.amigos.size(); i++) {
        std::string nid = idNodo(red.amigos[i].correo);
        ids[red.amigos[i].correo] = nid;
        d += "  " + nid + " [label=\"" + esc(red.amigos[i].nombre) +
             "\" fillcolor=\"#2563eb\" fontcolor=\"white\" shape=circle];\n";
    }
    for (size_t i = 0; i < red.enComun.size(); i++) {
        std::string nid = idNodo(red.enComun[i].correo);
        ids[red.enComun[i].correo] = nid;
        d += "  " + nid + " [label=\"" + esc(red.enComun[i].nombre) +
             "\" fillcolor=\"#9ca3af\" fontcolor=\"black\" shape=circle];\n";
    }
    for (size_t i = 0; i < red.aristas.size(); i++) {
        auto ia = ids.find(red.aristas[i].first);
        auto ib = ids.find(red.aristas[i].second);
        if (ia != ids.end() && ib != ids.end()) d += "  " + ia->second + " -- " + ib->second + ";\n";
    }
    d += "}\n";
    return d;
}

// El DOT del grafo completo: todos los vertices azules y todas las aristas.
std::string GrafoNoDirigido::toDotCompleto() const {
    std::string d = "graph RedSocial {\n";
    d += "  layout=neato; overlap=false; splines=true;\n";
    d += "  node [style=filled fillcolor=\"#2563eb\" fontcolor=\"white\" "
         "fontname=\"Helvetica\" shape=circle];\n";
    d += "  edge [color=\"#64748b\"];\n";
    std::map<std::string, std::string> ids;
    for (auto it = vertices_.begin(); it != vertices_.end(); ++it) {
        std::string nid = idNodo(it->first);
        ids[it->first] = nid;
        d += "  " + nid + " [label=\"" + esc(it->second.nombre) + "\"];\n";
    }
    std::vector<std::pair<std::string, std::string>> ar = todasLasAristas();
    for (size_t i = 0; i < ar.size(); i++) d += "  " + ids[ar[i].first] + " -- " + ids[ar[i].second] + ";\n";
    d += "}\n";
    return d;
}
