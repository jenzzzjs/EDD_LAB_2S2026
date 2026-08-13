#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <cstdlib>

using namespace std;

class Piloto {
private:
    std::string nombre;
    std::string nacionalidad;
    int horas_de_vuelo;

public:
    Piloto(const std::string& nombre, const std::string& nacionalidad, int horas_de_vuelo)
        : nombre(nombre), nacionalidad(nacionalidad), horas_de_vuelo(horas_de_vuelo) {}

    std::string getNombre() const { return nombre; }
    std::string getNacionalidad() const { return nacionalidad; }
    int getHorasDeVuelo() const { return horas_de_vuelo; }

    friend std::ostream& operator<<(std::ostream& os, const Piloto& piloto);
};

std::ostream& operator<<(std::ostream& os, const Piloto& piloto) {
    os << "Nombre: " << piloto.nombre << ", Nacionalidad: " << piloto.nacionalidad
        << ", Horas de Vuelo: " << piloto.horas_de_vuelo;
    return os;
}

class NodoArbolBinarioDeBusquedaPilotos {
private:
    Piloto piloto;
    NodoArbolBinarioDeBusquedaPilotos* izq;
    NodoArbolBinarioDeBusquedaPilotos* der;

public:
    NodoArbolBinarioDeBusquedaPilotos(const Piloto& piloto)
        : piloto(piloto), izq(nullptr), der(nullptr) {}

    Piloto getPiloto() const { return piloto; }
    NodoArbolBinarioDeBusquedaPilotos* getIzq() const { return izq; }
    NodoArbolBinarioDeBusquedaPilotos* getDer() const { return der; }

    void setPiloto(const Piloto& piloto) { this->piloto = piloto; }
    void setIzq(NodoArbolBinarioDeBusquedaPilotos* izq) { this->izq = izq; }
    void setDer(NodoArbolBinarioDeBusquedaPilotos* der) { this->der = der; }
};

class ArbolBinarioDeBusquedaPilotos {
private:
    NodoArbolBinarioDeBusquedaPilotos* raiz;

    NodoArbolBinarioDeBusquedaPilotos* insertarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, const Piloto& piloto);
    NodoArbolBinarioDeBusquedaPilotos* buscarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, int horas_de_vuelo);
    void preOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void inOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void postOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void generarDotAux(NodoArbolBinarioDeBusquedaPilotos* nodo, std::ofstream& archivo);
    void eliminarNodos(NodoArbolBinarioDeBusquedaPilotos* nodo);
    std::string quitarComillas(const std::string& campo);

public:
    ArbolBinarioDeBusquedaPilotos();
    bool estaVacio();
    void insertar(const Piloto& piloto);
    Piloto buscar(int horas_de_vuelo);
    void cargarCSV(const std::string& ruta);
    void preOrden();
    void inOrden();
    void postOrden();
    void generarDot();
    ~ArbolBinarioDeBusquedaPilotos();
};

ArbolBinarioDeBusquedaPilotos::ArbolBinarioDeBusquedaPilotos() {
    raiz = nullptr;
}

bool ArbolBinarioDeBusquedaPilotos::estaVacio() {
    return raiz == nullptr;
}

void ArbolBinarioDeBusquedaPilotos::insertar(const Piloto& piloto) {
    if (estaVacio()) {
        raiz = new NodoArbolBinarioDeBusquedaPilotos(piloto);
    }
    else {
        raiz = insertarNodo(raiz, piloto);
    }
}

NodoArbolBinarioDeBusquedaPilotos* ArbolBinarioDeBusquedaPilotos::insertarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, const Piloto& piloto) {
    if (nodo == nullptr) {
        return new NodoArbolBinarioDeBusquedaPilotos(piloto);
    }

    if (piloto.getHorasDeVuelo() == nodo->getPiloto().getHorasDeVuelo()) {
        return nodo;
    }
    else if (piloto.getHorasDeVuelo() < nodo->getPiloto().getHorasDeVuelo()) {
        nodo->setIzq(insertarNodo(nodo->getIzq(), piloto));
    }
    else {
        nodo->setDer(insertarNodo(nodo->getDer(), piloto));
    }

    return nodo;
}

std::string ArbolBinarioDeBusquedaPilotos::quitarComillas(const std::string& campo) {
    std::string resultado = campo;
    if (resultado.size() >= 2 && resultado.front() == '"' && resultado.back() == '"') {
        resultado = resultado.substr(1, resultado.size() - 2);
    }
    return resultado;
}

void ArbolBinarioDeBusquedaPilotos::cargarCSV(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo '" << ruta << "'." << std::endl;
        return;
    }

    std::string linea;
    bool encabezado = true;
    int insertados = 0;

    while (getline(archivo, linea)) {
        if (linea.empty()) {
            continue;
        }

        if (encabezado) {
            encabezado = false;
            continue;
        }

        size_t inicio = 0;
        std::vector<std::string> campos;
        for (size_t i = 0; i <= linea.size(); ++i) {
            if (i == linea.size() || linea[i] == ',') {
                campos.push_back(quitarComillas(linea.substr(inicio, i - inicio)));
                inicio = i + 1;
            }
        }

        if (campos.size() >= 3) {
            std::string nombre = campos[0];
            std::string nacionalidad = campos[1];
            int horas_de_vuelo = atoi(campos[2].c_str());
            insertar(Piloto(nombre, nacionalidad, horas_de_vuelo));
            insertados++;
        }
    }

    archivo.close();
    std::cout << "Se cargaron " << insertados << " pilotos desde '" << ruta << "'." << std::endl;
}

NodoArbolBinarioDeBusquedaPilotos* ArbolBinarioDeBusquedaPilotos::buscarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, int horas_de_vuelo) {
    if (nodo == nullptr || nodo->getPiloto().getHorasDeVuelo() == horas_de_vuelo) {
        return nodo;
    }

    if (horas_de_vuelo < nodo->getPiloto().getHorasDeVuelo()) {
        return buscarNodo(nodo->getIzq(), horas_de_vuelo);
    }
    else {
        return buscarNodo(nodo->getDer(), horas_de_vuelo);
    }
}

Piloto ArbolBinarioDeBusquedaPilotos::buscar(int horas_de_vuelo) {
    NodoArbolBinarioDeBusquedaPilotos* nodo = buscarNodo(raiz, horas_de_vuelo);
    if (nodo != nullptr) {
        return nodo->getPiloto();
    }
    else {
        throw std::runtime_error("Piloto no encontrado");
    }
}

void ArbolBinarioDeBusquedaPilotos::preOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    if (nodo != nullptr) {
        std::cout << nodo->getPiloto() << std::endl;
        preOrdenAux(nodo->getIzq());
        preOrdenAux(nodo->getDer());
    }
}

void ArbolBinarioDeBusquedaPilotos::preOrden() {
    preOrdenAux(raiz);
}

void ArbolBinarioDeBusquedaPilotos::inOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    if (nodo != nullptr) {
        inOrdenAux(nodo->getIzq());
        std::cout << nodo->getPiloto() << std::endl;
        inOrdenAux(nodo->getDer());
    }
}

void ArbolBinarioDeBusquedaPilotos::inOrden() {
    inOrdenAux(raiz);
}

void ArbolBinarioDeBusquedaPilotos::postOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    if (nodo != nullptr) {
        postOrdenAux(nodo->getIzq());
        postOrdenAux(nodo->getDer());
        std::cout << nodo->getPiloto() << std::endl;
    }
}

void ArbolBinarioDeBusquedaPilotos::postOrden() {
    postOrdenAux(raiz);
}

void ArbolBinarioDeBusquedaPilotos::generarDotAux(NodoArbolBinarioDeBusquedaPilotos* nodo, std::ofstream& archivo) {
    if (nodo != nullptr) {
        std::string nombreNodo = "\"" + nodo->getPiloto().getNombre() + "\\n" + std::to_string(nodo->getPiloto().getHorasDeVuelo()) + " horas\"";
        if (nodo->getIzq() != nullptr) {
            std::string nombreIzq = "\"" + nodo->getIzq()->getPiloto().getNombre() + "\\n" + std::to_string(nodo->getIzq()->getPiloto().getHorasDeVuelo()) + " horas\"";
            archivo << nombreNodo << " -> " << nombreIzq << ";\n";
        }
        if (nodo->getDer() != nullptr) {
            std::string nombreDer = "\"" + nodo->getDer()->getPiloto().getNombre() + "\\n" + std::to_string(nodo->getDer()->getPiloto().getHorasDeVuelo()) + " horas\"";
            archivo << nombreNodo << " -> " << nombreDer << ";\n";
        }
        generarDotAux(nodo->getIzq(), archivo);
        generarDotAux(nodo->getDer(), archivo);
    }
}

void ArbolBinarioDeBusquedaPilotos::generarDot() {
    std::ofstream archivo("arbol.dot");
    archivo << "digraph ArbolBinarioDeBusquedaPilotos {\n";
    archivo << "bgcolor=lightblue;\n";
    archivo << "Node [shape=square, style=filled, fillcolor=beige, color=black, penwidth=2];\n";
    archivo << "edge [splines=polyline, arrowhead=curve];\n";

    generarDotAux(raiz, archivo);
    archivo << "}\n";
    archivo.close();

    system("dot -Tpng arbol.dot -o arbol_binario_de_pilotos.png");
    system("start arbol_binario_de_pilotos.png");
}

ArbolBinarioDeBusquedaPilotos::~ArbolBinarioDeBusquedaPilotos() {
    eliminarNodos(raiz);
}

void ArbolBinarioDeBusquedaPilotos::eliminarNodos(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    if (nodo != nullptr) {
        eliminarNodos(nodo->getIzq());
        eliminarNodos(nodo->getDer());
        delete nodo;
    }
}

int main() {
    ArbolBinarioDeBusquedaPilotos arbol;
    int opcion;

    do {
        std::cout << std::endl;
        std::cout << "========== M E N U ===========" << std::endl;
        std::cout << "1. Insertar piloto" << std::endl;
        std::cout << "2. Cargar CSV" << std::endl;
        std::cout << "3. Ver pilotos" << std::endl;
        std::cout << "4. Generar reporte de pilotos" << std::endl;
        std::cout << "5. Salir" << std::endl;
        std::cout << "Ingrese una opcion: ";
        std::cin >> opcion;
        std::cin.ignore();

        switch (opcion) {
        case 1: {
            std::string nombre, nacionalidad;
            int horas_de_vuelo;

            std::cout << "Ingrese el nombre: ";
            std::getline(std::cin, nombre);
            std::cout << "Ingrese la nacionalidad: ";
            std::getline(std::cin, nacionalidad);
            std::cout << "Ingrese las horas de vuelo: ";
            std::cin >> horas_de_vuelo;
            std::cin.ignore();

            arbol.insertar(Piloto(nombre, nacionalidad, horas_de_vuelo));
            std::cout << "Piloto insertado correctamente." << std::endl;
            break;
        }
        case 2: {
            std::string ruta;
            std::cout << "Ingrese la ruta del archivo CSV: ";
            std::getline(std::cin, ruta);

            arbol.cargarCSV(ruta);
            break;
        }
        case 3:
            std::cout << "===== PILOTOS (IN ORDEN) =====" << std::endl;
            arbol.inOrden();
            break;
        case 4:
            arbol.generarDot();
            std::cout << "Reporte de pilotos generado correctamente." << std::endl;
            break;
        case 5:
            std::cout << "Saliendo del programa..." << std::endl;
            break;
        default:
            std::cout << "Opcion no valida." << std::endl;
            break;
        }
    } while (opcion != 5);

    return 0;
}
