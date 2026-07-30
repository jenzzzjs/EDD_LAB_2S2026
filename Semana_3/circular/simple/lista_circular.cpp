#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// nodo de la lista circular simple
// cada nodo guarda un amigo (nombre y edad) y apunta al siguiente
// el ultimo nodo apunta de vuelta al primero, formando un ciclo
struct Nodo {
    string nombre;
    int edad;
    Nodo* siguiente;

    Nodo(string nombre, int edad) {
        this->nombre = nombre;
        this->edad = edad;
        this->siguiente = nullptr;
    }
};

// maneja todos los nodos formando un ciclo
// la cabeza apunta al primer nodo, y el ultimo apunta a la cabeza
class ListaCircular {
private:
    Nodo* cabeza;

public:
    ListaCircular() {
        cabeza = nullptr;
    }

    // inserta un nuevo amigo al final de la lista
    // el ultimo nodo siempre apunta a la cabeza para mantener el ciclo
    void insertar(string nombre, int edad) {
        Nodo* nuevo = new Nodo(nombre, edad);

        if (cabeza == nullptr) {
            // si esta vacia, el nuevo nodo se apunta a si mismo
            cabeza = nuevo;
            nuevo->siguiente = cabeza;
        } else {
            // recorremos hasta el ultimo nodo (el que apunta a cabeza)
            Nodo* actual = cabeza;
            while (actual->siguiente != cabeza) {
                actual = actual->siguiente;
            }
            // conectamos el ultimo con el nuevo, y el nuevo con la cabeza
            actual->siguiente = nuevo;
            nuevo->siguiente = cabeza;
        }
        cout << "Amigo " << nombre << " agregado." << endl;
    }

    // elimina un amigo buscando por nombre
    // reconecta los punteros para mantener el ciclo cerrado
    void eliminar(string nombre) {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia." << endl;
            return;
        }

        // recorremos el ciclo buscando el nombre
        Nodo* actual = cabeza;
        Nodo* anterior = nullptr;

        do {
            if (actual->nombre == nombre) {
                if (actual == cabeza) {
                    // caso especial: eliminar la cabeza
                    if (actual->siguiente == cabeza) {
                        // unico nodo en la lista
                        cabeza = nullptr;
                    } else {
                        // buscamos el ultimo nodo para reconectar el ciclo
                        Nodo* ultimo = cabeza;
                        while (ultimo->siguiente != cabeza) {
                            ultimo = ultimo->siguiente;
                        }
                        cabeza = cabeza->siguiente;
                        ultimo->siguiente = cabeza;
                    }
                } else {
                    // saltamos el nodo actual
                    anterior->siguiente = actual->siguiente;
                }
                cout << "Amigo " << actual->nombre << " eliminado." << endl;
                delete actual;
                return;
            }
            // avanzamos al siguiente nodo
            anterior = actual;
            actual = actual->siguiente;
        } while (actual != cabeza);

        cout << "No se encontro amigo con nombre " << nombre << endl;
    }

    // genera un archivo .dot y lo compila a png con graphviz
    // dibuja los nodos en orden y una flecha del ultimo al primero
    void graficar() {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia, no hay nada que graficar." << endl;
            return;
        }

        ofstream archivo("simple/lista_circular.dot");
        archivo << "digraph ListaCircular {" << endl;
        archivo << "    rankdir=LR;" << endl;
        archivo << "    node [shape=record, style=filled, fillcolor=\"#eaff06\"];" << endl;

        Nodo* actual = cabeza;
        int i = 0;

        // creamos los nodos del grafo recorriendo el ciclo
        do {
            archivo << "    nodo" << i << " [label=\"{"
                    << actual->nombre << " | "
                    << actual->edad << " años"
                    << "}\"];" << endl;
            actual = actual->siguiente;
            i++;
        } while (actual != cabeza);

        // flechas entre nodos consecutivos
        for (int j = 0; j < i - 1; j++) {
            archivo << "    nodo" << j << " -> nodo" << j + 1 << ";" << endl;
        }
        // flecha del ultimo nodo al primero (cierre del ciclo)
        archivo << "    nodo" << i - 1 << " -> nodo0;" << endl;

        archivo << "}" << endl;
        archivo.close();

        int resultado = system("dot -Tpng simple/lista_circular.dot -o simple/lista_circular.png");
        if (resultado == 0) {
            cout << "Grafico generado: lista_circular.png" << endl;
        } else {
            cout << "Error: no se pudo generar el grafico. Verifica que Graphviz este instalado." << endl;
        }
    }

    // muestra todos los amigos en consola
    void mostrar() {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia." << endl;
            return;
        }

        // recorremos el ciclo imprimiendo cada amigo
        Nodo* actual = cabeza;
        do {
            cout << actual->nombre << ", " << actual->edad << " años" << endl;
            actual = actual->siguiente; // avanzamos al siguiente
        } while (actual != cabeza); // paramos cuando volvemos al inicio
    }

    // libera toda la memoria recorriendo el ciclo
    ~ListaCircular() {
        if (cabeza == nullptr) return;
        Nodo* actual = cabeza;
        Nodo* temp;
        do {
            temp = actual;
            actual = actual->siguiente;
            delete temp;
        } while (actual != cabeza);
    }
};

int main() {
    ListaCircular lista;
    int opcion;

    do {
        cout << "\n--- Lista Circular Simple de Amigos ---" << endl;
        cout << "1. Insertar amigo" << endl;
        cout << "2. Eliminar amigo" << endl;
        cout << "3. Graficar lista" << endl;
        cout << "4. Mostrar lista" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1) {
            string nombre;
            int edad;
            cout << "Nombre: ";
            cin >> nombre;
            cout << "Edad: ";
            cin >> edad;
            lista.insertar(nombre, edad);

        } else if (opcion == 2) {
            string nombre;
            cout << "Nombre del amigo a eliminar: ";
            cin >> nombre;
            lista.eliminar(nombre);

        } else if (opcion == 3) {
            lista.graficar();

        } else if (opcion == 4) {
            lista.mostrar();
        }
    } while (opcion != 0);

    return 0;
}
