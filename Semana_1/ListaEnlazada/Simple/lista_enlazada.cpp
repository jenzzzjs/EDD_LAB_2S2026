#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Nodo es la caja que guarda un dato y apunta al siguiente nodo
// Cada nodo tiene los datos del estudiante y un puntero al siguiente
struct Nodo {
    string nombre;
    int edad;
    string carnet;
    Nodo* siguiente;

    Nodo(string nombre, int edad, string carnet) {
        this->nombre = nombre;
        this->edad = edad;
        this->carnet = carnet;
        this->siguiente = nullptr;
    }
};

// ListaEnlazada maneja todos los nodos: agregar, eliminar y graficar
class ListaEnlazada {
private:
    Nodo* cabeza;

public:
    ListaEnlazada() {
        cabeza = nullptr;
    }

    // Agrega un nuevo estudiante al final de la lista
    void agregar(string nombre, int edad, string carnet) {
        Nodo* nuevo = new Nodo(nombre, edad, carnet);

        // Si la lista esta vacia, el nuevo nodo es la cabeza
        if (cabeza == nullptr) {
            cabeza = nuevo;
        } else {
            // Recorremos hasta el ultimo nodo
            Nodo* actual = cabeza;
            while (actual->siguiente != nullptr) {
                actual = actual->siguiente;
            }
            actual->siguiente = nuevo;
        }
        cout << "Estudiante " << nombre << " agregado." << endl;
    }

    // Elimina un estudiante buscando por carnet
    void eliminar(string carnet) {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia." << endl;
            return;
        }

        // Si el que hay que eliminar es la cabeza
        if (cabeza->carnet == carnet) {
            Nodo* temp = cabeza;
            cabeza = cabeza->siguiente;
            cout << "Estudiante " << temp->nombre << " eliminado." << endl;
            delete temp;
            return;
        }

        // Buscamos el nodo anterior al que queremos eliminar
        Nodo* actual = cabeza;
        while (actual->siguiente != nullptr && actual->siguiente->carnet != carnet) {
            actual = actual->siguiente;
        }

        if (actual->siguiente == nullptr) {
            cout << "No se encontro estudiante con carnet " << carnet << endl;
        } else {
            Nodo* temp = actual->siguiente;
            cout << "Estudiante " << temp->nombre << " eliminado." << endl;
            actual->siguiente = temp->siguiente;
            delete temp;
        }
    }

    // Genera un archivo .dot y lo compila a PNG con Graphviz
    void graficar() {
    if (cabeza == nullptr) {
        cout << "La lista esta vacia, no hay nada que graficar." << endl;
        return;
    }

    ofstream archivo("lista.dot");
    archivo << "digraph ListaEnlazada {" << endl;
    archivo << "    rankdir=LR;" << endl;
    // CAMBIA ESTA LÍNEA: agrega comillas dobles alrededor del color
    archivo << "    node [shape=record, style=filled, fillcolor=\"#eaff06\"];" << endl;

    Nodo* actual = cabeza;
    int i = 0;

    // Creamos los nodos del grafo
    while (actual != nullptr) {
        archivo << "    nodo" << i << " [label=\"{"
                << actual->carnet << " | "
                << actual->nombre << " | "
                << actual->edad << " años"
                << "}\"];" << endl;
        actual = actual->siguiente;
        i++;
    }

    // Creamos las flechas entre nodos
    for (int j = 0; j < i - 1; j++) {
        archivo << "    nodo" << j << " -> nodo" << j + 1 << ";" << endl;
    }

    archivo << "}" << endl;
    archivo.close();

    // Ejecutamos dot para generar la imagen
    int resultado = system("dot -Tpng lista.dot -o lista.png");
    if (resultado == 0) {
        cout << "Grafico generado: lista.png" << endl;
    } else {
        cout << "Error: no se pudo generar el grafico. Verifica que Graphviz este instalado." << endl;
    }
}

    // Muestra la lista en consola (opcional, para verificar)
    void mostrar() {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia." << endl;
            return;
        }

        Nodo* actual = cabeza;
        while (actual != nullptr) {
            cout << "[" << actual->carnet << "] " << actual->nombre << ", " << actual->edad << " años" << endl;
            actual = actual->siguiente;
        }
    }

    ~ListaEnlazada() {
        Nodo* actual = cabeza;
        while (actual != nullptr) {
            Nodo* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
    }
};

int main() {
    ListaEnlazada lista;
    int opcion;

    do {
        cout << "\n--- Lista Enlazada de Estudiantes ---" << endl;
        cout << "1. Agregar estudiante" << endl;
        cout << "2. Graficar lista" << endl;
        cout << "3. Eliminar estudiante" << endl;
        cout << "4. Mostrar lista" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            string nombre, carnet;
            int edad;
            cout << "Nombre: ";
            cin >> nombre;
            cout << "Edad (años): ";
            cin >> edad;
            cout << "Carnet: ";
            cin >> carnet;
            lista.agregar(nombre, edad, carnet);

        } else if (opcion == 2) {
            lista.graficar();

        } else if (opcion == 3) {
            string carnet;
            cout << "Carnet del estudiante a eliminar: ";
            cin >> carnet;
            lista.eliminar(carnet);

        } else if (opcion == 4) {
            lista.mostrar();
        }
    } while (opcion != 0);

    return 0;
}
