// Canciones.h - estructuras para guardar las canciones de cada artista
#ifndef CANCIONES_H
#define CANCIONES_H

#include <string>   // Para el tipo string
#include <vector>   // Para devolver las canciones en un vector
#include <utility>  // Para agrupar el nombre y el anio en un par
using namespace std;

struct NodoCancion {
    string nombre;     // Nombre de la cancion
    int anio;          // Anio de lanzamiento
    NodoCancion* siguiente; // Siguiente cancion de la cola

    // Constructor que recibe los datos y los guarda
    NodoCancion(string nombre, int anio) {
        this->nombre = nombre;
        this->anio = anio;
        this->siguiente = nullptr;
    }
};

class ColaCanciones {
private:
    NodoCancion* frente; // Primer nodo de la cola
    NodoCancion* final;  // Ultimo nodo de la cola
    int tamanio;         // Cantidad de canciones guardadas

public:
    // La cola inicia vacia con los punteros en nullptr
    ColaCanciones() {
        frente = nullptr;
        final = nullptr;
        tamanio = 0;
    }

    // Destructor: recorre la cola desde el frente y borra cada nodo
    ~ColaCanciones() {
        while (frente != nullptr) {
            NodoCancion* temp = frente;
            frente = frente->siguiente;
            delete temp;
        }
    }

    // Encola una cancion al final de la cola (FIFO)
    bool encolar(string nombre, int anio) {
        NodoCancion* nueva = new NodoCancion(nombre, anio);

        if (frente == nullptr) {
            // La cola esta vacia, el nuevo nodo es el primero
            frente = nueva;
            final = nueva;
        } else {
            // Se enlaza despues del final actual y se actualiza el puntero
            final->siguiente = nueva;
            final = nueva;
        }

        tamanio++;
        return true;
    }

    // Recorre la cola y devuelve todas las canciones en un vector
    vector<pair<string, int>> obtenerTodasCanciones() {
        vector<pair<string, int>> canciones;
        NodoCancion* actual = frente;

        while (actual != nullptr) {
            canciones.push_back(make_pair(actual->nombre, actual->anio));
            actual = actual->siguiente;
        }

        return canciones;
    }

    // Desencola la primera cancion (FIFO) y devuelve sus datos por referencia
    bool desencolar(string& nombre, int& anio) {
        if (frente == nullptr) {
            return false; // La cola esta vacia
        }

        NodoCancion* temp = frente;
        nombre = temp->nombre;
        anio = temp->anio;

        frente = frente->siguiente;
        if (frente == nullptr) {
            final = nullptr; // Si quedo vacia, el final se reinicia
        }

        delete temp;
        tamanio--;
        return true;
    }

    // Devuelve cuantas canciones tiene la cola
    int obtenerTamanio() {
        return tamanio;
    }
};

#endif
