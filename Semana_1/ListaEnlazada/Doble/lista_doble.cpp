#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Nodo con puntero al siguiente y al anterior
// Esto permite recorrer la lista en ambas direcciones
struct Nodo {
    string nombre;
    int edad;
    string carnet;
    Nodo* siguiente;
    Nodo* anterior;

    Nodo(string nombre, int edad, string carnet) {
        this->nombre = nombre;
        this->edad = edad;
        this->carnet = carnet;
        this->siguiente = nullptr;
        this->anterior = nullptr;
    }
};

// Lista doblemente enlazada: cada nodo conoce al siguiente y al anterior
class ListaDoble {
private:
    Nodo* cabeza;

public:
    ListaDoble() {
        cabeza = nullptr;
    }

    // Agrega un nuevo estudiante al final de la lista
    void agregar(string nombre, int edad, string carnet) {
        Nodo* nuevo = new Nodo(nombre, edad, carnet);

        if (cabeza == nullptr) {
            cabeza = nuevo;
        } else {
            Nodo* actual = cabeza;
            while (actual->siguiente != nullptr) {
                actual = actual->siguiente;
            }
            // Aqui esta la diferencia con la simple: conectamos en ambas direcciones
            actual->siguiente = nuevo;
            nuevo->anterior = actual;
        }
        cout << "Estudiante " << nombre << " agregado." << endl;
    }

    // Elimina un estudiante buscando por carnet
    void eliminar(string carnet) {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia." << endl;
            return;
        }

        Nodo* actual = cabeza;
        while (actual != nullptr && actual->carnet != carnet) {
            actual = actual->siguiente;
        }

        if (actual == nullptr) {
            cout << "No se encontro estudiante con carnet " << carnet << endl;
            return;
        }

        cout << "Estudiante " << actual->nombre << " eliminado." << endl;

        // Si es la cabeza, movemos la cabeza
        if (actual == cabeza) {
            cabeza = actual->siguiente;
            if (cabeza != nullptr) {
                cabeza->anterior = nullptr;
            }
        } else {
            // Reconectamos el anterior y el siguiente
            actual->anterior->siguiente = actual->siguiente;
            if (actual->siguiente != nullptr) {
                actual->siguiente->anterior = actual->anterior;
            }
        }

        delete actual;
    }

    // Genera un archivo .dot y lo compila a PNG con Graphviz
    void graficar() {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia, no hay nada que graficar." << endl;
            return;
        }

        ofstream archivo("lista.dot");
        archivo << "digraph ListaDoble {" << endl;
        archivo << "    rankdir=LR;" << endl;
        archivo << "    node [shape=record, style=filled, fillcolor=\"#ff2f2f\"];" << endl;

        Nodo* actual = cabeza;
        int i = 0;

        // Creamos los nodos del grafo
        while (actual != nullptr) {
            archivo << "    nodo" << i << " [label=\"{"
                    << actual->carnet << " | "
                    << actual->nombre << " | "
                    << actual->edad << " meses"
                    << "}\"];" << endl;
            actual = actual->siguiente;
            i++;
        }

        // Flechas dobles entre nodos (dir=both)
        for (int j = 0; j < i - 1; j++) {
            archivo << "    nodo" << j << " -> nodo" << j + 1 << " [dir=both];" << endl;
        }

        archivo << "}" << endl;
        archivo.close();

        int resultado = system("dot -Tpng lista.dot -o lista.png");
        if (resultado == 0) {
            cout << "Grafico generado: lista.png" << endl;
        } else {
            cout << "Error: no se pudo generar el grafico. Verifica que Graphviz este instalado." << endl;
        }
    }

    // Muestra la lista en consola de cabeza a cola
    void mostrar() {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia." << endl;
            return;
        }

        cout << "--- De inicio a fin ---" << endl;
        Nodo* actual = cabeza;
        while (actual != nullptr) {
            cout << "[" << actual->carnet << "] " << actual->nombre << ", " << actual->edad << " meses" << endl;
            actual = actual->siguiente;
        }
    }

    // Destructor: libera toda la memoria
    ~ListaDoble() {
        Nodo* actual = cabeza;
        while (actual != nullptr) {
            Nodo* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
    }
};

int main() {
    ListaDoble lista;
    int opcion;

    do {
        cout << "\n--- Lista Doble de Estudiantes ---" << endl;
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
            cout << "Edad (meses): ";
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
