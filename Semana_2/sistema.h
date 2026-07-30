#ifndef SISTEMA_H
#define SISTEMA_H

#include "listaenlazada.h" // Para usar la lista enlazada de estudiantes

// Sistema actua como capa intermedia entre la vista y la lista enlazada
// Permite insertar, buscar y eliminar estudiantes sin acceder directamente a la lista
class Sistema {
private:
    ListaEnlazada lista; // Lista que almacena todos los estudiantes

public:
    void insertar(string nombre, string carnet, string carrera);
    Nodo* buscar(string carnet);
    bool eliminar(string carnet);
    ListaEnlazada& getLista();
};

#endif