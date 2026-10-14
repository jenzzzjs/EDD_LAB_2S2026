#pragma once
// Grafo NO dirigido con lista de adyacencia hecha a mano.
// Sin networkx ni ninguna libreria de grafos: solo map + vector y
// recorridos con ciclos normales, como se pide en el laboratorio.

#include <map>
#include <string>
#include <utility>
#include <vector>

// Un vertice es una persona: su nombre para mostrar y su correo,
// que usamos como identificador unico (no se repite).
struct Vertice {
    std::string nombre;
    std::string correo;
};

// Un "amigo en comun" es alguien que NO es mi amigo directo, pero que
// es amigo de al menos uno de mis amigos. Guardamos con quien conecta
// para poder dibujar las aristas despues.
struct EnComun {
    std::string nombre;
    std::string correo;
    std::vector<std::string> conectaCon;
};

// La red de un usuario vista en 2 niveles: el, sus amigos directos
// y los amigos en comun (los amigos de sus amigos).
struct RedUsuario {
    bool existe = false;
    Vertice yo;
    std::vector<Vertice> amigos;       // directos (en el dibujo van azules)
    std::vector<EnComun> enComun;      // segundo nivel (en el dibujo van grises)
    std::vector<std::pair<std::string, std::string>> aristas;
};

class GrafoNoDirigido {
public:
    bool existeVertice(const std::string& correo) const;

    // Mete a una persona al grafo. Devuelve false si el correo ya estaba.
    bool agregarVertice(const std::string& nombre, const std::string& correo);

    // Dice si dos correos ya son amigos (o sea, si hay arista entre ellos).
    bool sonAmigos(const std::string& a, const std::string& b) const;

    // Crea la amistad a--b. Como el grafo NO es dirigido, la arista se
    // guarda en los dos sentidos: a apunta a b y b apunta a a.
    bool agregarArista(const std::string& a, const std::string& b);

    // Todos los amigos directos de alguien, ordenados por nombre.
    std::vector<Vertice> obtenerAmigos(const std::string& correo) const;

    // Amigos que a y b tienen en comun (interseccion de sus listas, a manita).
    std::vector<Vertice> amigosEnComun(const std::string& a, const std::string& b) const;

    // Arma la red de 2 niveles de un usuario para dibujarla despues.
    RedUsuario redDeUsuario(const std::string& correo) const;

    // Todas las aristas del grafo sin repetir (a--b sale una sola vez).
    std::vector<std::pair<std::string, std::string>> todasLasAristas() const;
    std::vector<Vertice> todosLosVertices() const;

    // DOT para Graphviz. Colores de la red: yo=verde, amigos=azul,
    // amigos en comun=gris (asi lo pide el enunciado).
    std::string toDotRed(const std::string& correo) const;
    std::string toDotCompleto() const;

private:
    // La lista de adyacencia: cada correo tiene su lista de vecinos.
    // Si Ana es amiga de Bruno, "ana" aparece en la lista de "bruno"
    // y "bruno" en la de "ana", porque la amistad va en los dos sentidos.
    std::map<std::string, Vertice> vertices_;              // correo -> persona
    std::map<std::string, std::vector<std::string>> ady_;  // correo -> vecinos

    // Busca un valor dentro de un vector con un ciclo normal.
    static bool contiene(const std::vector<std::string>& v, const std::string& x);

    // Convierte un correo en un ID valido para el DOT (los correos tienen
    // @ y puntos y eso Graphviz no lo acepta como nombre de nodo).
    static std::string idNodo(const std::string& correo);

    // Escapa comillas y backslash para no romper el DOT.
    static std::string esc(const std::string& s);
};
