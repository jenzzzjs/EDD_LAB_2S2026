#ifndef LISTAENLAZADA_H
#define LISTAENLAZADA_H

#include <string> // Para usar string
using namespace std;

// Nodo es la caja que guarda los datos de un estudiante y apunta al siguiente nodo
// Cada nodo tiene nombre, carnet, carrera y un puntero al siguiente
struct Nodo {
    string nombre;
    string carnet;
    string carrera;
    Nodo* siguiente;

    Nodo(string nombre, string carnet, string carrera);
};

// ListaEnlazada maneja todos los nodos: insertar, buscar, eliminar y graficar
class ListaEnlazada {
private:
    Nodo* cabeza; // Puntero al primer nodo de la lista

public:
    ListaEnlazada();
    ~ListaEnlazada(); // Destructor: libera toda la memoria

    void insertar(string nombre, string carnet, string carrera);
    Nodo* buscar(string carnet);
    bool eliminar(string carnet);
    void graficar();
    Nodo* getCabeza();
};

#endif