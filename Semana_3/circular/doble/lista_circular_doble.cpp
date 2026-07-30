#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// nodo de la lista circular doble
// cada nodo guarda un amigo y tiene punteros al siguiente y al anterior
// el ciclo se cierra en ambos sentidos: siguiente y anterior
struct Nodo {
    string nombre;
    int edad;
    Nodo* siguiente;
    Nodo* anterior;

    Nodo(string nombre, int edad) {
        this->nombre = nombre;
        this->edad = edad;
        this->siguiente = nullptr;
        this->anterior = nullptr;
    }
};

// cada nodo apunta al siguiente y al anterior
// el ultimo nodo apunta a la cabeza como siguiente, y la cabeza apunta al ultimo como anterior
class ListaCircularDoble {
private:
    Nodo* cabeza;

public:
    ListaCircularDoble() {
        cabeza = nullptr;
    }

    // inserta un nuevo amigo al final de la lista
    // conecta en ambas direcciones y mantiene el ciclo cerrado
    void insertar(string nombre, int edad) {
        Nodo* nuevo = new Nodo(nombre, edad);

        if (cabeza == nullptr) {
            // si esta vacia, el nuevo nodo se apunta a si mismo en ambos sentidos
            cabeza = nuevo;
            nuevo->siguiente = nuevo;
            nuevo->anterior = nuevo;
        } else {
            // el ultimo nodo es cabeza->anterior por ser circular
            Nodo* ultimo = cabeza->anterior;
            // conectamos el ultimo con el nuevo
            ultimo->siguiente = nuevo;
            nuevo->anterior = ultimo;
            // conectamos el nuevo con la cabeza
            nuevo->siguiente = cabeza;
            cabeza->anterior = nuevo;
        }
        cout << "Amigo " << nombre << " agregado." << endl;
    }

    // elimina un amigo buscando por nombre
    // reconecta los punteros siguiente y anterior del nodo removido
    void eliminar(string nombre) {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia." << endl;
            return;
        }

        // recorremos el ciclo buscando el nombre
        Nodo* actual = cabeza;
        do {
            if (actual->nombre == nombre) {
                if (actual == cabeza && actual->siguiente == cabeza) {
                    // unico nodo en la lista
                    cabeza = nullptr;
                } else {
                    // reconectamos el nodo anterior y el siguiente entre si
                    actual->anterior->siguiente = actual->siguiente;
                    actual->siguiente->anterior = actual->anterior;
                    if (actual == cabeza) {
                        // si eliminamos la cabeza, movemos la cabeza al siguiente
                        cabeza = actual->siguiente;
                    }
                }
                cout << "Amigo " << actual->nombre << " eliminado." << endl;
                delete actual;
                return;
            }
            // avanzamos al siguiente nodo
            actual = actual->siguiente;
        } while (actual != cabeza);

        cout << "No se encontro amigo con nombre " << nombre << endl;
    }

    // genera un archivo .dot y lo compila a png con graphviz
    // usa dir=both para mostrar las conexiones en ambos sentidos
    void graficar() {
        if (cabeza == nullptr) {
            cout << "La lista esta vacia, no hay nada que graficar." << endl;
            return;
        }

        ofstream archivo("doble/lista_circular_doble.dot");
        archivo << "digraph ListaCircularDoble {" << endl;
        archivo << "    rankdir=LR;" << endl;
        archivo << "    node [shape=record, style=filled, fillcolor=\"#ff2f2f\"];" << endl;

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

        // flechas bidireccionales entre nodos consecutivos
        for (int j = 0; j < i - 1; j++) {
            archivo << "    nodo" << j << " -> nodo" << j + 1 << " [dir=both];" << endl;
        }
        // flecha bidireccional del ultimo al primero (cierre del ciclo)
        archivo << "    nodo" << i - 1 << " -> nodo0 [dir=both];" << endl;

        archivo << "}" << endl;
        archivo.close();

        int resultado = system("dot -Tpng doble/lista_circular_doble.dot -o doble/lista_circular_doble.png");
        if (resultado == 0) {
            cout << "Grafico generado: lista_circular_doble.png" << endl;
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
    ~ListaCircularDoble() {
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
    ListaCircularDoble lista;
    int opcion;

    do {
        cout << "\n--- Lista Circular Doble de Amigos ---" << endl;
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
