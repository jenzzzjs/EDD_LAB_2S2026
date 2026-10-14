#include "Grafo.h"

// --- Ayudas internas ---

// Recorre la lista comparando uno por uno. Asi de simple, porque la idea
// es que la estructura se entienda a puro ciclo.
bool GrafoNoDirigido::contiene(const Lista<std::string>& l, const std::string& x) {
    for (size_t i = 0; i < l.tam(); i++)
        if (l[i] == x) return true;
    return false;
}

// Posicion del vertice / entrada de adyacencia, o -1 si no esta.
int GrafoNoDirigido::idxVertice(const std::string& correo) const {
    for (size_t i = 0; i < vertices_.tam(); i++)
        if (vertices_[i].correo == correo) return (int)i;
    return -1;
}

int GrafoNoDirigido::idxAdy(const std::string& correo) const {
    for (size_t i = 0; i < ady_.tam(); i++)
        if (ady_[i].correo == correo) return (int)i;
    return -1;
}

// Ordena una lista de vertices por nombre con burbuja (a mano).
static void ordenarVertices(Lista<Vertice>& l) {
    for (size_t i = 0; i < l.tam(); i++) {
        for (size_t j = 0; j + 1 < l.tam() - i; j++) {
            if (l[j + 1].nombre < l[j].nombre) {
                Vertice t = l[j];
                l[j] = l[j + 1];
                l[j + 1] = t;
            }
        }
    }
}

// Busca un correo en una lista de pares correo->id (para el DOT).
static std::string buscarId(const Lista<Par>& ids, const std::string& correo) {
    for (size_t i = 0; i < ids.tam(); i++)
        if (ids[i].clave == correo) return ids[i].valor;
    return "";
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
    return idxVertice(correo) >= 0;
}

// Meter un vertice es crear su ficha y dejarle su lista de vecinos vacia,
// porque recien registrado nadie es amigo de nadie todavia.
bool GrafoNoDirigido::agregarVertice(const std::string& nombre, const std::string& correo) {
    if (existeVertice(correo)) return false;
    vertices_.push_back(Vertice{nombre, correo});
    AdyEntrada e;
    e.correo = correo;
    ady_.push_back(e);
    return true;
}

// --- Aristas (la amistad) ---

bool GrafoNoDirigido::sonAmigos(const std::string& a, const std::string& b) const {
    int ia = idxAdy(a);
    if (ia < 0) return false;
    return contiene(ady_[ia].vecinos, b);
}

// Aceptar una solicitud termina aqui: se agrega la arista en AMBOS sentidos
// porque la amistad no tiene direccion (si Ana es amiga de Bruno, Bruno es
// amigo de Ana). No se permiten bucles (a--a) ni aristas repetidas.
bool GrafoNoDirigido::agregarArista(const std::string& a, const std::string& b) {
    if (a == b) return false;
    int ia = idxAdy(a);
    int ib = idxAdy(b);
    if (ia < 0 || ib < 0) return false;
    if (contiene(ady_[ia].vecinos, b)) return false;
    ady_[ia].vecinos.push_back(b);
    ady_[ib].vecinos.push_back(a);
    return true;
}

// --- Consultas ---

// Los amigos directos no son mas que la lista de vecinos del vertice.
// Se ordenan por nombre para que el frontend los muestre siempre igual.
Lista<Vertice> GrafoNoDirigido::obtenerAmigos(const std::string& correo) const {
    Lista<Vertice> out;
    int ia = idxAdy(correo);
    if (ia < 0) return out;
    const Lista<std::string>& vec = ady_[ia].vecinos;
    for (size_t i = 0; i < vec.tam(); i++) {
        int iv = idxVertice(vec[i]);
        if (iv >= 0) out.push_back(vertices_[iv]);
    }
    ordenarVertices(out);
    return out;
}

// Amigos en comun = los que aparecen en las dos listas de vecinos.
// Se hace con doble ciclo (para cada vecino de a, se busca en los de b)
// y se evita meter duplicados.
Lista<Vertice> GrafoNoDirigido::amigosEnComun(const std::string& a,
                                              const std::string& b) const {
    Lista<Vertice> out;
    int ia = idxAdy(a);
    int ib = idxAdy(b);
    if (ia < 0 || ib < 0) return out;
    const Lista<std::string>& va = ady_[ia].vecinos;
    const Lista<std::string>& vb = ady_[ib].vecinos;
    for (size_t i = 0; i < va.tam(); i++) {
        for (size_t j = 0; j < vb.tam(); j++) {
            if (va[i] == vb[j]) {
                int iv = idxVertice(va[i]);
                if (iv >= 0) {
                    bool dup = false;
                    for (size_t k = 0; k < out.tam(); k++)
                        if (out[k].correo == vertices_[iv].correo) dup = true;
                    if (!dup) out.push_back(vertices_[iv]);
                }
            }
        }
    }
    ordenarVertices(out);
    return out;
}

// Revisa si la arista a--b ya esta guardada, sin importar el orden en que
// se guardo (sirve porque el grafo no es dirigido).
static bool mismaArista(const Arista& e, const std::string& a, const std::string& b) {
    return (e.a == a && e.b == b) || (e.a == b && e.b == a);
}

// Arma la red de 2 niveles: yo, mis amigos, y los amigos de mis amigos que
// todavia no son mis amigos (esos son los "en comun"). De paso junta todas
// las aristas que hay que dibujar: yo--amigo y amigo--en_comun. Si dos de
// mis amigos son amigos entre ellos, esa arista tambien se dibuja.
RedUsuario GrafoNoDirigido::redDeUsuario(const std::string& correo) const {
    RedUsuario red;
    int iv = idxVertice(correo);
    if (iv < 0) return red;
    red.existe = true;
    red.yo = vertices_[iv];
    red.amigos = obtenerAmigos(correo);

    Lista<std::string> correosAmigos;
    for (size_t i = 0; i < red.amigos.tam(); i++) {
        correosAmigos.push_back(red.amigos[i].correo);
        red.aristas.push_back(Arista{correo, red.amigos[i].correo});
    }

    // Segundo nivel: se visitan los vecinos de cada amigo, uno por uno.
    for (size_t i = 0; i < red.amigos.tam(); i++) {
        const std::string& ca = red.amigos[i].correo;
        int ia = idxAdy(ca);
        if (ia < 0) continue;
        const Lista<std::string>& vec = ady_[ia].vecinos;
        for (size_t j = 0; j < vec.tam(); j++) {
            const std::string& v = vec[j];
            if (v == correo) continue;  // soy yo, no me dibujo dos veces
            if (contiene(correosAmigos, v)) {
                // Es amigo directo tambien: solo falta la arista amigo--amigo.
                bool ya = false;
                for (size_t k = 0; k < red.aristas.tam(); k++)
                    if (mismaArista(red.aristas[k], ca, v)) ya = true;
                if (!ya) red.aristas.push_back(Arista{ca, v});
                continue;
            }
            int ivv = idxVertice(v);
            if (ivv < 0) continue;
            // Si es la primera vez que lo vemos, se crea su ficha de "en comun".
            // Si ya estaba, solo se anota con que otro amigo conecta.
            int idx = -1;
            for (size_t k = 0; k < red.enComun.tam(); k++)
                if (red.enComun[k].correo == v) idx = (int)k;
            if (idx < 0) {
                EnComun ec;
                ec.nombre = vertices_[ivv].nombre;
                ec.correo = v;
                ec.conectaCon.push_back(ca);
                red.enComun.push_back(ec);
            } else if (!contiene(red.enComun[idx].conectaCon, ca)) {
                red.enComun[idx].conectaCon.push_back(ca);
            }
            bool ya = false;
            for (size_t k = 0; k < red.aristas.tam(); k++)
                if (mismaArista(red.aristas[k], ca, v)) ya = true;
            if (!ya) red.aristas.push_back(Arista{ca, v});
        }
    }
    // Ordena los en comun por nombre con burbuja.
    for (size_t i = 0; i < red.enComun.tam(); i++) {
        for (size_t j = 0; j + 1 < red.enComun.tam() - i; j++) {
            if (red.enComun[j + 1].nombre < red.enComun[j].nombre) {
                EnComun t = red.enComun[j];
                red.enComun[j] = red.enComun[j + 1];
                red.enComun[j + 1] = t;
            }
        }
    }
    return red;
}

// Recorre toda la adyacencia y junta cada arista una sola vez (si ya salio
// a--b, no se vuelve a meter b--a).
Lista<Arista> GrafoNoDirigido::todasLasAristas() const {
    Lista<Arista> out;
    for (size_t e = 0; e < ady_.tam(); e++) {
        const std::string& a = ady_[e].correo;
        const Lista<std::string>& vec = ady_[e].vecinos;
        for (size_t i = 0; i < vec.tam(); i++) {
            const std::string& b = vec[i];
            bool dup = false;
            for (size_t k = 0; k < out.tam(); k++)
                if (mismaArista(out[k], a, b)) dup = true;
            if (!dup) out.push_back(Arista{a, b});
        }
    }
    return out;
}

Lista<Vertice> GrafoNoDirigido::todosLosVertices() const {
    Lista<Vertice> out;
    for (size_t i = 0; i < vertices_.tam(); i++) out.push_back(vertices_[i]);
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
    Lista<Par> ids;
    std::string idYo = idNodo(red.yo.correo);
    ids.push_back(Par{red.yo.correo, idYo});
    d += "  " + idYo + " [label=\"" + esc(red.yo.nombre) +
         "\" fillcolor=\"#22c55e\" fontcolor=\"white\" shape=circle];\n";
    for (size_t i = 0; i < red.amigos.tam(); i++) {
        std::string nid = idNodo(red.amigos[i].correo);
        ids.push_back(Par{red.amigos[i].correo, nid});
        d += "  " + nid + " [label=\"" + esc(red.amigos[i].nombre) +
             "\" fillcolor=\"#2563eb\" fontcolor=\"white\" shape=circle];\n";
    }
    for (size_t i = 0; i < red.enComun.tam(); i++) {
        std::string nid = idNodo(red.enComun[i].correo);
        ids.push_back(Par{red.enComun[i].correo, nid});
        d += "  " + nid + " [label=\"" + esc(red.enComun[i].nombre) +
             "\" fillcolor=\"#9ca3af\" fontcolor=\"black\" shape=circle];\n";
    }
    for (size_t i = 0; i < red.aristas.tam(); i++) {
        std::string ia = buscarId(ids, red.aristas[i].a);
        std::string ib = buscarId(ids, red.aristas[i].b);
        if (!ia.empty() && !ib.empty()) d += "  " + ia + " -- " + ib + ";\n";
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
    Lista<Par> ids;
    for (size_t i = 0; i < vertices_.tam(); i++) {
        std::string nid = idNodo(vertices_[i].correo);
        ids.push_back(Par{vertices_[i].correo, nid});
        d += "  " + nid + " [label=\"" + esc(vertices_[i].nombre) + "\"];\n";
    }
    Lista<Arista> ar = todasLasAristas();
    for (size_t i = 0; i < ar.tam(); i++)
        d += "  " + buscarId(ids, ar[i].a) + " -- " + buscarId(ids, ar[i].b) + ";\n";
    d += "}\n";
    return d;
}
