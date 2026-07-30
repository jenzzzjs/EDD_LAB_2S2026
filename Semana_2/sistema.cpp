#include "sistema.h" // Cabecera del sistema

// Inserta un estudiante en la lista enlazada
void Sistema::insertar(string nombre, string carnet, string carrera) {
    lista.insertar(nombre, carnet, carrera);
}

// Busca un estudiante por carnet y retorna el nodo encontrado
Nodo* Sistema::buscar(string carnet) {
    return lista.buscar(carnet);
}

// Elimina un estudiante por carnet, retorna true si se elimino correctamente
bool Sistema::eliminar(string carnet) {
    return lista.eliminar(carnet);
}

// Retorna una referencia a la lista enlazada para operaciones externas
ListaEnlazada& Sistema::getLista() {
    return lista;
}