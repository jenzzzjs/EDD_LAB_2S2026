#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

// ArregloDinamico reemplaza al vector de la STL con codigo propio
// Es un arreglo que crece solo cuando se llena y permite acceder por indice
template <typename T>
class ArregloDinamico {
private:
    T* datos;
    int capacidad;
    int cantidad;

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

// Pelicula guarda la informacion basica de cada pelicula: su codigo y su nombre
class Pelicula {
private:
    std::string codigo;
    std::string nombre;

public:
    Pelicula(const std::string& codigo, const std::string& nombre)
        : codigo(codigo), nombre(nombre) {}

    std::string getCodigo() const { return codigo; }
    std::string getNombre() const { return nombre; }

    friend std::ostream& operator<<(std::ostream& os, const Pelicula& pelicula);
};

// permite imprimir una pelicula de forma legible
std::ostream& operator<<(std::ostream& os, const Pelicula& pelicula) {
    os << "Codigo: " << pelicula.codigo << ", Nombre: " << pelicula.nombre;
    return os;
}

// NodoArbolPeliculas es la caja que guarda cada pelicula dentro del arbol
// Cada nodo apunta a su hijo izquierdo (codigo menor) y a su hijo derecho (codigo mayor)
class NodoArbolPeliculas {
private:
    Pelicula pelicula;
    NodoArbolPeliculas* izq;
    NodoArbolPeliculas* der;

public:
    NodoArbolPeliculas(const Pelicula& pelicula)
        : pelicula(pelicula), izq(nullptr), der(nullptr) {}

    Pelicula getPelicula() const { return pelicula; }
    NodoArbolPeliculas* getIzq() const { return izq; }
    NodoArbolPeliculas* getDer() const { return der; }

    void setPelicula(const Pelicula& pelicula) { this->pelicula = pelicula; }
    void setIzq(NodoArbolPeliculas* izq) { this->izq = izq; }
    void setDer(NodoArbolPeliculas* der) { this->der = der; }
};

// ArbolBinarioDeBusquedaPeliculas es el arbol que organiza las peliculas por su codigo
// Las peliculas con codigo menor van a la izquierda y las de codigo mayor a la derecha
class ArbolBinarioDeBusquedaPeliculas {
private:
    NodoArbolPeliculas* raiz;

    NodoArbolPeliculas* insertarNodo(NodoArbolPeliculas* nodo, const Pelicula& pelicula);
    void generarDotAux(NodoArbolPeliculas* nodo, std::ofstream& archivo);
    void eliminarNodos(NodoArbolPeliculas* nodo);
    std::string quitarComillas(const std::string& campo);
    int numeroCodigo(const std::string& codigo) const;

public:
    ArbolBinarioDeBusquedaPeliculas();
    bool estaVacio();
    void insertar(const Pelicula& pelicula);
    void cargarCSV(const std::string& ruta);
    void generarDot();
    ~ArbolBinarioDeBusquedaPeliculas();
};

// el arbol empieza vacio, sin raiz
ArbolBinarioDeBusquedaPeliculas::ArbolBinarioDeBusquedaPeliculas() {
    raiz = nullptr;
}

// dice si el arbol esta vacio o no
bool ArbolBinarioDeBusquedaPeliculas::estaVacio() {
    return raiz == nullptr;
}

// inserta una pelicula en el arbol, respetando el orden por codigo
void ArbolBinarioDeBusquedaPeliculas::insertar(const Pelicula& pelicula) {
    // si el arbol esta vacio, la pelicula se convierte en la raiz
    if (estaVacio()) {
        raiz = new NodoArbolPeliculas(pelicula);
    }
    // si no, buscamos el lugar correcto
    else {
        raiz = insertarNodo(raiz, pelicula);
    }
}

// recorre el arbol desde un nodo hasta encontrar el lugar correcto para la nueva pelicula
NodoArbolPeliculas* ArbolBinarioDeBusquedaPeliculas::insertarNodo(NodoArbolPeliculas* nodo, const Pelicula& pelicula) {
    // si encontramos un lugar vacio, creamos el nodo nuevo
    if (nodo == nullptr) {
        return new NodoArbolPeliculas(pelicula);
    }

    // si ya existe una pelicula con el mismo codigo, no la duplicamos
    if (numeroCodigo(pelicula.getCodigo()) == numeroCodigo(nodo->getPelicula().getCodigo())) {
        return nodo;
    }
    // si el codigo es menor, la pelicula va por el hijo izquierdo
    else if (numeroCodigo(pelicula.getCodigo()) < numeroCodigo(nodo->getPelicula().getCodigo())) {
        nodo->setIzq(insertarNodo(nodo->getIzq(), pelicula));
    }
    // si el codigo es mayor, la pelicula va por el hijo derecho
    else {
        nodo->setDer(insertarNodo(nodo->getDer(), pelicula));
    }

    return nodo;
}

// convierte el codigo en un numero para poder compararlo (P004 = 4, P0018 = 18)
int ArbolBinarioDeBusquedaPeliculas::numeroCodigo(const std::string& codigo) const {
    std::string digitos;
    // se quedan solo los digitos del codigo
    for (char c : codigo) {
        if (std::isdigit(c)) {
            digitos += c;
        }
    }
    // si el codigo no tenia numeros se devuelve 0
    return digitos.empty() ? 0 : std::atoi(digitos.c_str());
}

// quita las comillas que a veces traen los campos del CSV
std::string ArbolBinarioDeBusquedaPeliculas::quitarComillas(const std::string& campo) {
    std::string resultado = campo;
    if (resultado.size() >= 2 && resultado.front() == '"' && resultado.back() == '"') {
        resultado = resultado.substr(1, resultado.size() - 2);
    }
    return resultado;
}

// lee el archivo CSV y mete todas las peliculas al arbol
void ArbolBinarioDeBusquedaPeliculas::cargarCSV(const std::string& ruta) {
    std::ifstream archivo(ruta);
    // si no se pudo abrir el archivo, avisamos y salimos
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo '" << ruta << "'." << std::endl;
        return;
    }

    std::string linea;
    bool encabezado = true;
    int insertados = 0;

    // se lee el archivo linea por linea
    while (getline(archivo, linea)) {
        // si la linea esta vacia la ignoramos
        if (linea.empty()) {
            continue;
        }

        // la primera linea es el encabezado (codigo,nombre) y se ignora
        if (encabezado) {
            encabezado = false;
            continue;
        }

        // separamos la linea por comas para obtener el codigo y el nombre
        size_t inicio = 0;
        ArregloDinamico<std::string> campos;
        for (size_t i = 0; i <= linea.size(); ++i) {
            if (i == linea.size() || linea[i] == ',') {
                campos.agregar(quitarComillas(linea.substr(inicio, i - inicio)));
                inicio = i + 1;
            }
        }

        // si la linea tiene por lo menos 2 campos, creamos la pelicula y la insertamos
        if (campos.size() >= 2) {
            std::string codigo = campos[0];
            std::string nombre = campos[1];
            insertar(Pelicula(codigo, nombre));
            insertados++;
        }
    }

    archivo.close();
    std::cout << "Se cargaron " << insertados << " peliculas desde '" << ruta << "'." << std::endl;
}

// escribe en el archivo la relacion del nodo con sus hijos
// se llama de forma recursiva para recorrer todo el arbol
void ArbolBinarioDeBusquedaPeliculas::generarDotAux(NodoArbolPeliculas* nodo, std::ofstream& archivo) {
    if (nodo != nullptr) {
        // el nombre del nodo es el nombre de la pelicula con su codigo
        std::string nombreNodo = "\"" + nodo->getPelicula().getNombre() + "\\n(" + nodo->getPelicula().getCodigo() + ")\"";
        // si tiene hijo izquierdo, se dibuja la flecha hacia el
        if (nodo->getIzq() != nullptr) {
            std::string nombreIzq = "\"" + nodo->getIzq()->getPelicula().getNombre() + "\\n(" + nodo->getIzq()->getPelicula().getCodigo() + ")\"";
            archivo << nombreNodo << " -> " << nombreIzq << ";\n";
        }
        // si tiene hijo derecho, se dibuja la flecha hacia el
        if (nodo->getDer() != nullptr) {
            std::string nombreDer = "\"" + nodo->getDer()->getPelicula().getNombre() + "\\n(" + nodo->getDer()->getPelicula().getCodigo() + ")\"";
            archivo << nombreNodo << " -> " << nombreDer << ";\n";
        }
        // se repite con los hijos para recorrer todo el arbol
        generarDotAux(nodo->getIzq(), archivo);
        generarDotAux(nodo->getDer(), archivo);
    }
}

// genera el archivo .dot con el codigo de Graphviz y lo convierte en una imagen
void ArbolBinarioDeBusquedaPeliculas::generarDot() {
    std::ofstream archivo("arbol_peliculas.dot");
    // cabecera del codigo DOT con el formato del grafo
    archivo << "digraph ArbolBinarioDeBusquedaPeliculas {\n";
    archivo << "bgcolor=lightblue;\n";
    archivo << "Node [shape=square, style=filled, fillcolor=beige, color=black, penwidth=2];\n";
    archivo << "edge [splines=polyline, arrowhead=curve];\n";

    // se escriben todos los nodos y sus conexiones
    generarDotAux(raiz, archivo);
    archivo << "}\n";
    archivo.close();

    // se convierte el .dot en una imagen y se abre
    system("dot -Tpng arbol_peliculas.dot -o arbol_peliculas.png");
    system("start arbol_peliculas.png");
}

// al terminar el programa se libera la memoria de todos los nodos del arbol
ArbolBinarioDeBusquedaPeliculas::~ArbolBinarioDeBusquedaPeliculas() {
    eliminarNodos(raiz);
}

// recorre el arbol por la izquierda y la derecha y elimina cada nodo
void ArbolBinarioDeBusquedaPeliculas::eliminarNodos(NodoArbolPeliculas* nodo) {
    if (nodo != nullptr) {
        eliminarNodos(nodo->getIzq());
        eliminarNodos(nodo->getDer());
        delete nodo;
    }
}

// programa principal: muestra el menu y deja elegir la opcion
int main() {
    ArbolBinarioDeBusquedaPeliculas arbol;
    int opcion;

    // el menu se repite hasta que el usuario elija salir
    do {
        std::cout << std::endl;
        std::cout << "========== M E N U ===========" << std::endl;
        std::cout << "1. Insertar pelicula" << std::endl;
        std::cout << "2. Graficar arbol" << std::endl;
        std::cout << "3. Cargar CSV con peliculas" << std::endl;
        std::cout << "0. Salir" << std::endl;
        std::cout << "Ingrese una opcion: ";
        std::cin >> opcion;
        std::cin.ignore();

        switch (opcion) {
        // opcion 1: pedimos el codigo y el nombre, y la insertamos
        case 1: {
            std::string codigo, nombre;

            std::cout << "Ingrese el codigo de la pelicula: ";
            std::getline(std::cin, codigo);
            std::cout << "Ingrese el nombre de la pelicula: ";
            std::getline(std::cin, nombre);

            arbol.insertar(Pelicula(codigo, nombre));
            std::cout << "Pelicula insertada correctamente." << std::endl;
            break;
        }
        // opcion 2: se genera la imagen del arbol
        case 2:
            arbol.generarDot();
            std::cout << "Arbol graficado correctamente." << std::endl;
            break;
        // opcion 3: pedimos la ruta del CSV y cargamos las peliculas
        case 3: {
            std::string ruta;
            std::cout << "Ingrese la ruta del archivo CSV: ";
            std::getline(std::cin, ruta);

            arbol.cargarCSV(ruta);
            break;
        }
        // opcion 0: terminamos el programa
        case 0:
            std::cout << "Saliendo del programa..." << std::endl;
            break;
        default:
            std::cout << "Opcion no valida." << std::endl;
            break;
        }
    } while (opcion != 0);

    return 0;
}
