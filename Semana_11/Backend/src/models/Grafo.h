#pragma once
// Grafo NO dirigido con lista de adyacencia hecha a mano.
// Sin std::vector, sin std::map ni ninguna libreria de grafos: solo la
// Lista enlazada simple (vertices, entradas de adyacencia y vecinos) y
// recorridos con ciclos normales, como se pide en el laboratorio.

#include <string>

#include "Lista.h"

// Un vertice es una persona: su nombre para mostrar y su correo,
// que usamos como identificador unico (no se repite).
struct Vertice {
    std::string nombre;
    std::string correo;
};

// Una arista no dirigida a--b (la amistad va en los dos sentidos,
// asi que no importa el orden en que se guarde).
struct Arista {
    std::string a;
    std::string b;
};

// Una entrada de la lista de adyacencia: un correo y su lista
// enlazada de vecinos.
struct AdyEntrada {
    std::string correo;
    Lista<std::string> vecinos;
};

// Un "amigo en comun" es alguien que NO es mi amigo directo, pero que
// es amigo de al menos uno de mis amigos. Guardamos con quien conecta
// para poder dibujar las aristas despues.
struct EnComun {
    std::string nombre;
    std::string correo;
    Lista<std::string> conectaCon;
};

// La red de un usuario vista en 2 niveles: el, sus amigos directos
// y los amigos en comun (los amigos de sus amigos).
struct RedUsuario {
    bool existe = false;
    Vertice yo;
    Lista<Vertice> amigos;    // directos (en el dibujo van azules)
    Lista<EnComun> enComun;   // segundo nivel (en el dibujo van grises)
    Lista<Arista> aristas;
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
    Lista<Vertice> obtenerAmigos(const std::string& correo) const;

    // Amigos que a y b tienen en comun (interseccion de sus listas, a manita).
    Lista<Vertice> amigosEnComun(const std::string& a, const std::string& b) const;

    // Arma la red de 2 niveles de un usuario para dibujarla despues.
    RedUsuario redDeUsuario(const std::string& correo) const;

    // Todas las aristas del grafo sin repetir (a--b sale una sola vez).
    Lista<Arista> todasLasAristas() const;
    Lista<Vertice> todosLosVertices() const;

    // DOT para Graphviz. Colores de la red: yo=verde, amigos=azul,
    // amigos en comun=gris (asi lo pide el enunciado).
    std::string toDotRed(const std::string& correo) const;
    std::string toDotCompleto() const;

private:
    // Vertices y adyacencia como listas enlazadas: cada correo aparece
    // en su entrada de adyacencia con sus vecinos. Si Ana es amiga de
    // Bruno, "ana" esta en los vecinos de "bruno" y "bruno" en los de
    // "ana", porque la amistad va en los dos sentidos.
    Lista<Vertice> vertices_;    // fichas de personas
    Lista<AdyEntrada> ady_;      // correo -> vecinos

    // Posicion en cada lista o -1 si no esta (busqueda lineal a mano).
    int idxVertice(const std::string& correo) const;
    int idxAdy(const std::string& correo) const;

    // Busca un valor dentro de una lista con un ciclo normal.
    static bool contiene(const Lista<std::string>& l, const std::string& x);

    // Convierte un correo en un ID valido para el DOT (los correos tienen
    // @ y puntos y eso Graphviz no lo acepta como nombre de nodo).
    static std::string idNodo(const std::string& correo);

    // Escapa comillas y backslash para no romper el DOT.
    static std::string esc(const std::string& s);
};
