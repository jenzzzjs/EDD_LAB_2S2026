// ArregloDinamico.h - arreglo de tamaño dinámico implementado a mano
#ifndef ARREGLO_DINAMICO_H
#define ARREGLO_DINAMICO_H

// ArregloDinamico reemplaza al vector de la STL con codigo propio
// Es un arreglo que crece solo cuando se llena y permite acceder por indice
template <typename T>
class ArregloDinamico {
private:
    T* datos;      // bloque de memoria donde viven los elementos
    int capacidad; // cuantos elementos caben en el bloque actual
    int cantidad;  // cuantos elementos hay guardados

public:
    // El arreglo arranca vacio, sin bloque de memoria asignado
    ArregloDinamico() : datos(nullptr), capacidad(0), cantidad(0) {}

    // Libera la memoria cuando se destruye el arreglo
    ~ArregloDinamico() {
        delete[] datos;
    }

    // Copia el contenido de otro arreglo cuando se crea uno nuevo
    ArregloDinamico(const ArregloDinamico& otro) : datos(nullptr), capacidad(0), cantidad(0) {
        copiar(otro);
    }

    // Copia el contenido de otro arreglo cuando se asigna uno a otro
    ArregloDinamico& operator=(const ArregloDinamico& otro) {
        if (this != &otro) {
            delete[] datos;
            datos = nullptr;
            capacidad = 0;
            cantidad = 0;
            copiar(otro);
        }
        return *this;
    }

    // Agrega un elemento al final y crece si hace falta
    void agregar(const T& valor) {
        if (cantidad == capacidad) {
            int nuevaCapacidad = capacidad == 0 ? 4 : capacidad * 2;
            T* nuevo = new T[nuevaCapacidad];
            for (int i = 0; i < cantidad; i++) {
                nuevo[i] = datos[i];
            }
            delete[] datos;
            datos = nuevo;
            capacidad = nuevaCapacidad;
        }
        datos[cantidad] = valor;
        cantidad++;
    }

    // Devuelve el elemento que esta en la posicion indicada
    T& operator[](int posicion) {
        return datos[posicion];
    }

    // Version constante que solo deja leer el elemento
    const T& operator[](int posicion) const {
        return datos[posicion];
    }

    // Cantidad de elementos guardados
    int size() const {
        return cantidad;
    }

    // True si el arreglo esta vacio
    bool empty() const {
        return cantidad == 0;
    }

private:
    // Copia los datos de otro arreglo hacia este
    void copiar(const ArregloDinamico& otro) {
        cantidad = otro.cantidad;
        capacidad = otro.cantidad;
        datos = capacidad > 0 ? new T[capacidad] : nullptr;
        for (int i = 0; i < cantidad; i++) {
            datos[i] = otro.datos[i];
        }
    }
};

#endif
