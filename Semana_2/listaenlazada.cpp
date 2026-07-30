#include "listaenlazada.h" // Cabecera de la lista enlazada
#include <fstream> // Para escribir archivos como DOT
#include <cstdlib> // Para ejecutar comandos del sistema como Graphviz
#include <iostream> // para mensajes en consola

// Constructor del Nodo: inicializa los datos del estudiante y apunta a nullptr
Nodo::Nodo(string nombre, string carnet, string carrera) {
    this->nombre = nombre;
    this->carnet = carnet;
    this->carrera = carrera;
    this->siguiente = nullptr;
}

// Constructor de ListaEnlazada: crea una lista vacia
ListaEnlazada::ListaEnlazada() {
    cabeza = nullptr;
}

// Destructor: libera toda la memoria de los nodos
ListaEnlazada::~ListaEnlazada() {
    Nodo* actual = cabeza;
    while (actual != nullptr) {
        Nodo* temp = actual;
        actual = actual->siguiente;
        delete temp;
    }
}

// Inserta un nuevo estudiante al final de la lista
void ListaEnlazada::insertar(string nombre, string carnet, string carrera) {
    Nodo* nuevo = new Nodo(nombre, carnet, carrera);

    // Si la lista esta vacia, el nuevo nodo es la cabeza
    if (cabeza == nullptr) {
        cabeza = nuevo;
    } else {
        // Recorremos hasta el ultimo nodo para agregar al final
        Nodo* actual = cabeza;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevo;
    }
}

// Busca un estudiante por carnet y retorna el nodo si lo encuentra, nullptr si no
Nodo* ListaEnlazada::buscar(string carnet) {
    Nodo* actual = cabeza;
    while (actual != nullptr) {
        if (actual->carnet == carnet) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

// Elimina un estudiante por carnet, retorna true si lo elimino, false si no lo encontro
bool ListaEnlazada::eliminar(string carnet) {
    if (cabeza == nullptr) return false;

    // Si el que hay que eliminar es la cabeza
    if (cabeza->carnet == carnet) {
        Nodo* temp = cabeza;
        cabeza = cabeza->siguiente;
        delete temp;
        return true;
    }

    // Buscamos el nodo anterior al que queremos eliminar
    Nodo* actual = cabeza;
    while (actual->siguiente != nullptr && actual->siguiente->carnet != carnet) {
        actual = actual->siguiente;
    }

    if (actual->siguiente == nullptr) return false;

    // Desconectamos el nodo y liberamos memoria
    Nodo* temp = actual->siguiente;
    actual->siguiente = temp->siguiente;
    delete temp;
    return true;
}

// Genera un archivo .dot y lo compila a PNG con Graphviz
void ListaEnlazada::graficar() {
    if (cabeza == nullptr) {
        cout << "[DEBUG] La lista esta vacia, no se genera reporte." << endl;
        return;
    }

    // Rutas para los archivos DOT y PNG
    string ruta = "../Semana_2/";
    string archivoDOT = ruta + "lista.dot";
    string archivoPNG = ruta + "lista.png";

    // Buscamos Graphviz en las rutas comunes de instalacion
    string dotPath = "";
    string rutasPosibles[] = {
        "dot",
        "C:/Program Files/Graphviz/bin/dot.exe",
        "C:/Program Files (x86)/Graphviz/bin/dot.exe"
    };
    for (string& r : rutasPosibles) {
        string testCmd = "\"" + r + "\" -V 2>nul";
        if (system(testCmd.c_str()) == 0) {
            dotPath = r;
            break;
        }
    }

    if (dotPath.empty()) {
        cout << "[ERROR] No se encontro Graphviz (dot). Instale Graphviz." << endl;
        return;
    }

    cout << "[DEBUG] Graphviz encontrado en: " << dotPath << endl;
    cout << "[DEBUG] Generando archivo DOT: " << archivoDOT << endl;

    // Abrimos el archivo DOT para escribir la estructura del grafo
    ofstream archivo(archivoDOT);
    if (!archivo.is_open()) {
        cout << "[ERROR] No se pudo abrir " << archivoDOT << endl;
        return;
    }

    // Configuracion del grafo: direccion de izquierda a derecha, nodos con estilo record
    archivo << "digraph ListaEnlazada {" << endl;
    archivo << "    rankdir=LR;" << endl;
    archivo << "    node [shape=record, style=filled, fillcolor=\"#eaff06\"];" << endl;

    // Creamos los nodos del grafo con los datos del estudiante
    Nodo* actual = cabeza;
    int i = 0;
    while (actual != nullptr) {
        archivo << "    nodo" << i << " [label=\"{"
                << actual->carnet << " | "
                << actual->nombre << " | "
                << actual->carrera
                << "}\"];" << endl;
        actual = actual->siguiente;
        i++;
    }

    // Creamos las flechas entre nodos consecutivos
    for (int j = 0; j < i - 1; j++) {
        archivo << "    nodo" << j << " -> nodo" << j + 1 << ";" << endl;
    }

    archivo << "}" << endl;
    archivo.close();

    cout << "[DEBUG] Archivo DOT generado con " << i << " nodos." << endl;

    // Ejecutamos dot para generar la imagen PNG
    string cmd = "\"\"" + dotPath + "\" -Tpng \"" + archivoDOT + "\" -o \"" + archivoPNG + "\"\"";
    cout << "[DEBUG] Comando: " << cmd << endl;

    int resultado = system(cmd.c_str());
    cout << "[DEBUG] Codigo de retorno de dot: " << resultado << endl;

    if (resultado == 0) {
        cout << "[DEBUG] PNG generado: " << archivoPNG << endl;
    } else {
        cout << "[ERROR] Graphviz fallo con codigo: " << resultado << endl;
    }
}

// Retorna el puntero a la cabeza de la lista
Nodo* ListaEnlazada::getCabeza() {
    return cabeza;
}