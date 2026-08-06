#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <sstream> 
#include <vector>


using json = nlohmann::json;
using namespace std;

class Avion {
public:
    std::string vuelo;
    std::string numero_de_registro;
    std::string modelo;
    int capacidad;
    std::string aerolinea;
    std::string ciudad_destino;
    std::string estado;
    Avion* siguiente;
    Avion* anterior;

    Avion() : siguiente(nullptr), anterior(nullptr) {}

    Avion(const std::string& vuelo, const std::string& numero_de_registro, const std::string& modelo, int capacidad, const std::string& aerolinea, const std::string& ciudad_destino, const std::string& estado)
        : vuelo(vuelo), numero_de_registro(numero_de_registro), modelo(modelo), capacidad(capacidad), aerolinea(aerolinea), ciudad_destino(ciudad_destino), estado(estado), siguiente(nullptr), anterior(nullptr) {}

    std::string getNumeroDeRegistro() const {
        return numero_de_registro;
    }

    std::string getVuelo() const {
        return vuelo;
    }

    std::string getCiudadDestino() const {
        return ciudad_destino;
    }

    void imprimir() const {
        std::cout << "Vuelo: " << vuelo << ", Numero de Registro: " << numero_de_registro
            << ", Estado: " << estado << std::endl;
    }
};


class Piloto {
private:
    std::string nombre;
    std::string nacionalidad;
    std::string numero_de_id;
    std::string vuelo;
    int horas_de_vuelo;
    std::string tipo_de_licencia;

public:
    Piloto(const std::string& nombre, const std::string& nacionalidad, const std::string& numero_de_id, const std::string& vuelo, int horas_de_vuelo, const std::string& tipo_de_licencia)
        : nombre(nombre), nacionalidad(nacionalidad), numero_de_id(numero_de_id), vuelo(vuelo), horas_de_vuelo(horas_de_vuelo), tipo_de_licencia(tipo_de_licencia) {}

    
    std::string getNombre() const { return nombre; }
    std::string getVuelo() const { return vuelo; }
    int getHorasDeVuelo() const { return horas_de_vuelo; }
    std::string getNumeroDeId() const { return numero_de_id; }

    
    friend std::ostream& operator<<(std::ostream& os, const Piloto& piloto);
};



std::ostream& operator<<(std::ostream& os, const Piloto& piloto) {
    os << "Nombre: " << piloto.nombre << ", Nacionalidad: " << piloto.nacionalidad
        << ", Número de ID: " << piloto.numero_de_id << ", Vuelo: " << piloto.vuelo
        << ", Horas de Vuelo: " << piloto.horas_de_vuelo << ", Tipo de Licencia: " << piloto.tipo_de_licencia;
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
    NodoArbolBinarioDeBusquedaPilotos* eliminarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, const std::string& id_piloto, bool& eliminado);
    NodoArbolBinarioDeBusquedaPilotos* encontrarMinimo(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void preOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void inOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void postOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void generarDotAux(NodoArbolBinarioDeBusquedaPilotos* nodo, std::ofstream& archivo);
    void eliminarNodos(NodoArbolBinarioDeBusquedaPilotos* nodo);

public:
    ArbolBinarioDeBusquedaPilotos();
    bool estaVacio();
    void insertar(const Piloto& piloto);
    Piloto buscar(int horas_de_vuelo);
    void eliminar(const std::string& id_piloto);
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

NodoArbolBinarioDeBusquedaPilotos* ArbolBinarioDeBusquedaPilotos::encontrarMinimo(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    while (nodo->getIzq() != nullptr) {
        nodo = nodo->getIzq();
    }
    return nodo;
}


NodoArbolBinarioDeBusquedaPilotos* ArbolBinarioDeBusquedaPilotos::eliminarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, const std::string& id_piloto, bool& eliminado) {
    if (nodo == nullptr) {
        return nodo;
    }

    if (id_piloto < nodo->getPiloto().getNumeroDeId()) {
        nodo->setIzq(eliminarNodo(nodo->getIzq(), id_piloto, eliminado));
    }
    else if (id_piloto > nodo->getPiloto().getNumeroDeId()) {
        nodo->setDer(eliminarNodo(nodo->getDer(), id_piloto, eliminado));
    }
    else {
        eliminado = true; 
       
        if (nodo->getIzq() == nullptr) {
            NodoArbolBinarioDeBusquedaPilotos* temp = nodo->getDer();
            delete nodo;
            return temp;
        }
        else if (nodo->getDer() == nullptr) {
            NodoArbolBinarioDeBusquedaPilotos* temp = nodo->getIzq();
            delete nodo;
            return temp;
        }

       
        NodoArbolBinarioDeBusquedaPilotos* temp = encontrarMinimo(nodo->getDer());
        nodo->setPiloto(temp->getPiloto());
        nodo->setDer(eliminarNodo(nodo->getDer(), temp->getPiloto().getNumeroDeId(), eliminado));
    }

   
    nodo->setDer(eliminarNodo(nodo->getDer(), id_piloto, eliminado));
    nodo->setIzq(eliminarNodo(nodo->getIzq(), id_piloto, eliminado));

    return nodo;
}

void ArbolBinarioDeBusquedaPilotos::eliminar(const std::string& id_piloto) {
    bool eliminado = false;
    do {
        eliminado = false;
        raiz = eliminarNodo(raiz, id_piloto, eliminado);
    } while (eliminado);
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
        std::cout << "Horas de vuelo: " << nodo->getPiloto().getHorasDeVuelo() << std::endl;
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
        std::cout << "Horas de vuelo: " << nodo->getPiloto().getHorasDeVuelo() << std::endl;
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
        std::cout << "Horas de vuelo: " << nodo->getPiloto().getHorasDeVuelo() << std::endl;
    }
}

void ArbolBinarioDeBusquedaPilotos::postOrden() {
    postOrdenAux(raiz);
}

void ArbolBinarioDeBusquedaPilotos::generarDotAux(NodoArbolBinarioDeBusquedaPilotos* nodo, std::ofstream& archivo) {
    if (nodo != nullptr) {
        if (nodo->getIzq() != nullptr) {
            archivo << "\"" << nodo->getPiloto().getHorasDeVuelo() << " horas\""
                << " -> \"" << nodo->getIzq()->getPiloto().getHorasDeVuelo() << " horas\";\n";
        }
        if (nodo->getDer() != nullptr) {
            archivo << "\"" << nodo->getPiloto().getHorasDeVuelo() << " horas\""
                << " -> \"" << nodo->getDer()->getPiloto().getHorasDeVuelo() << " horas\";\n";
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







class NodoHash {
public:
    Piloto piloto;
    NodoHash* siguiente;

    NodoHash(const Piloto& p) : piloto(p), siguiente(nullptr) {}
};

class TablaHashPilotos {
private:
    std::vector<NodoHash*> table;
    int M;

    int hashFunction(const std::string& key) {
        int hashValue = 0;
        for (char ch : key) {
            hashValue += ch;
        }
        return hashValue % M;
    }

public:
    TablaHashPilotos(int size = 18) : M(size) {
        table.resize(M, nullptr);
    }

    void insertar(const Piloto& piloto) {
        int index = hashFunction(piloto.getNumeroDeId());
        NodoHash* nuevoNodo = new NodoHash(piloto);
        if (table[index] == nullptr) {
            table[index] = nuevoNodo;
        }
        else {
            NodoHash* temp = table[index];
            while (temp->siguiente != nullptr) {
                temp = temp->siguiente;
            }
            temp->siguiente = nuevoNodo;
        }
    }

    Piloto buscar(const std::string& numero_de_id) {
        int index = hashFunction(numero_de_id);
        NodoHash* temp = table[index];
        while (temp != nullptr) {
            if (temp->piloto.getNumeroDeId() == numero_de_id) {
                return temp->piloto;
            }
            temp = temp->siguiente;
        }
        throw std::runtime_error("Piloto no encontrado");
    }

    void imprimirTabla() {
        for (int i = 0; i < M; ++i) {
            std::cout << "Indice " << i << ": ";
            NodoHash* temp = table[i];
            while (temp != nullptr) {
                std::cout << temp->piloto << " -> ";
                temp = temp->siguiente;
            }
            std::cout << "nullptr" << std::endl;
        }
    }


    void eliminar(const std::string& numero_de_id) {
        int index = hashFunction(numero_de_id);
        NodoHash* temp = table[index];
        NodoHash* prev = nullptr;

        while (temp != nullptr) {
            if (temp->piloto.getNumeroDeId() == numero_de_id) {
              
                if (prev == nullptr) {
                    table[index] = temp->siguiente;
                }
                else {
                    prev->siguiente = temp->siguiente; 
                }
                delete temp; 
                return;
            }
            prev = temp;
            temp = temp->siguiente;
        }

        throw std::runtime_error("Piloto no encontrado para eliminar");
    }





    void generarDot(const std::string& filename) {
        std::ofstream dotFile(filename);
        if (!dotFile.is_open()) {
            std::cerr << "No se pudo crear el archivo DOT." << std::endl;
            return;
        }

        dotFile << "digraph TablaHashPilotos {" << std::endl;
        dotFile << "  Node [shape=record];" << std::endl;

        for (int i = 0; i < M; ++i) {
            dotFile << "  subgraph cluster_" << i << " {" << std::endl;
            dotFile << "    label=\"Índice " << i << "\";" << std::endl;

            dotFile << "    index_" << i << " [label=\"<f0> " << i << "\"];" << std::endl;

            NodoHash* temp = table[i];
            int NodeCount = 0;
            while (temp != nullptr) {
                dotFile << "    Node_" << i << "_" << NodeCount << " [label=\"{ ID: " << temp->piloto.getNumeroDeId() << " | Nombre: " << temp->piloto.getNombre() << " }\"];" << std::endl;
                if (NodeCount > 0) {
                    dotFile << "    Node_" << i << "_" << (NodeCount - 1) << " -> Node_" << i << "_" << NodeCount << ";" << std::endl;
                }
                else {
                    dotFile << "    index_" << i << " -> Node_" << i << "_" << NodeCount << ";" << std::endl;
                }
                temp = temp->siguiente;
                NodeCount++;
            }

            dotFile << "  }" << std::endl;
        }

        dotFile << "}" << std::endl;
        dotFile.close();

        
        std::string command = "dot -Tpng " + filename + " -o " + filename + ".png";
        system(command.c_str());

        std::string openCommand = "start " + filename + ".png";
        system(openCommand.c_str());
    }




    ~TablaHashPilotos() {
        for (int i = 0; i < M; ++i) {
            NodoHash* temp = table[i];
            while (temp != nullptr) {
                NodoHash* toDelete = temp;
                temp = temp->siguiente;
                delete toDelete;
            }
        }
    }
};




class ListaCircularlistaMantenimiento {
private:
    Avion* inicio;

public:
    ListaCircularlistaMantenimiento() : inicio(nullptr) {}

    void agregarAvionEnMantenimiento(const Avion& avion) {
        Avion* nuevoAvion = new Avion(avion);
        if (inicio == nullptr) {
            inicio = nuevoAvion;
            inicio->siguiente = inicio;
            inicio->anterior = inicio;
        }
        else {
            Avion* ultimo = inicio->anterior;
            ultimo->siguiente = nuevoAvion;
            nuevoAvion->anterior = ultimo;
            nuevoAvion->siguiente = inicio;
            inicio->anterior = nuevoAvion;
        }
    }

    void mostrarlistaMantenimiento() const {
        if (inicio == nullptr) {
            std::cout << "No hay aviones en mantenimiento en la lista." << std::endl;
            return;
        }

        Avion* temp = inicio;
        do {
            std::cout << "Avion en mantenimiento - Vuelo: " << temp->vuelo << ", Numero de Registro: " << temp->numero_de_registro << ", Estado: " << temp->estado << std::endl;
            temp = temp->siguiente;
        } while (temp != inicio);
    }

    void GenerarDot() {
        std::ofstream archivo("aviones_en_mantenimiento.dot");
        archivo << "digraph ListaCircularlistaMantenimiento {\n";
        archivo << "bgcolor=lightblue;\n";
        archivo << "Node [shape=box style=filled fillcolor=beige fontcolor=black];\n";
        archivo << "edge [splines=ortho];\n";

        if (inicio != nullptr) {
            Avion* temp = inicio;
            do {
                archivo << "\"" << temp->numero_de_registro << "\" [label=\"" << temp->numero_de_registro << "\\nEstado: " << temp->estado << "\"];\n";
                archivo << "\"" << temp->numero_de_registro << "\" -> \"" << temp->siguiente->numero_de_registro << "\";\n";
                temp = temp->siguiente;
            } while (temp != inicio);
        }

        archivo << "}\n";
        archivo.close();

        system("dot -Tpng aviones_en_mantenimiento.dot -o aviones_en_mantenimiento.png");
        system("start aviones_en_mantenimiento.png");
    }

    bool eliminarAvionPorRegistro(const std::string& numero_de_registro, Avion& avionEliminado) {
        if (inicio == nullptr) {
            return false;
        }

        Avion* temp = inicio;
        do {
            if (temp->numero_de_registro == numero_de_registro) {
                if (temp == inicio && temp->siguiente == inicio) {
                    inicio = nullptr;
                }
                else {
                    temp->anterior->siguiente = temp->siguiente;
                    temp->siguiente->anterior = temp->anterior;
                    if (temp == inicio) {
                        inicio = temp->siguiente;
                    }
                }

                avionEliminado = *temp;
                delete temp;
                return true;
            }
            temp = temp->siguiente;
        } while (temp != inicio);

        return false;
    }
};


class BNodoDisponibles {
public:
    BNodoDisponibles(int m); 
    ~BNodoDisponibles();     

    
    bool esHoja() const;
    void insertarClave(const Avion& nuevoAvion);
    BNodoDisponibles* dividirNodo(Avion& mediana);
    int encontrarPosicionInsertar(const std::string& numeroDeRegistro);
    bool estaLleno() const;

    
    void eliminarClave(const std::string& numeroDeRegistro);
    void eliminarClaveHoja(int idx);
    void eliminarClaveNoHoja(int idx);
    Avion obtenerPredecesor(int idx);
    Avion obtenerSucesor(int idx);
    void tomarPrestadoDeAnterior(int idx);
    void tomarPrestadoDeSiguiente(int idx);
    void fusionar(int idx);
    int buscarClave(const std::string& numeroDeRegistro);

private:
    int clavesUsadas;  
    Avion* clave;       
    BNodoDisponibles** puntero;   
    BNodoDisponibles* padre;      
    BNodoDisponibles* siguiente;   
    bool hoja;        
    int m;             

    friend class ArbolBAvionesDisponibles;
};

typedef BNodoDisponibles* pBNodoDisponibles;

BNodoDisponibles::BNodoDisponibles(int m)
    : clavesUsadas(0), clave(new Avion[m - 1]), puntero(new pBNodoDisponibles[m]), padre(nullptr), siguiente(nullptr), hoja(true), m(m) {
    for (int i = 0; i < m; ++i) {
        puntero[i] = nullptr;
    }
}

BNodoDisponibles::~BNodoDisponibles() {
    delete[] clave;
    delete[] puntero;
}

bool BNodoDisponibles::esHoja() const {
    return hoja;
}

void BNodoDisponibles::insertarClave(const Avion& nuevoAvion) {
    int i = encontrarPosicionInsertar(nuevoAvion.getNumeroDeRegistro());
    for (int j = clavesUsadas; j > i; --j) {
        clave[j] = clave[j - 1];
    }
    clave[i] = nuevoAvion;
    ++clavesUsadas;
}

BNodoDisponibles* BNodoDisponibles::dividirNodo(Avion& mediana) {
    int mid = clavesUsadas / 2;
    BNodoDisponibles* nuevoNodo = new BNodoDisponibles(m);
    nuevoNodo->hoja = hoja;

    for (int i = mid + 1; i < clavesUsadas; ++i) {
        nuevoNodo->clave[i - (mid + 1)] = clave[i];
    }

    if (!hoja) {
        for (int i = mid + 1; i <= clavesUsadas; ++i) {
            nuevoNodo->puntero[i - (mid + 1)] = puntero[i];
            puntero[i] = nullptr; 
        }
    }

    nuevoNodo->clavesUsadas = clavesUsadas - mid - 1;
    clavesUsadas = mid;

    mediana = clave[mid];
    nuevoNodo->siguiente = this->siguiente;
    this->siguiente = nuevoNodo;

    return nuevoNodo;
}


int BNodoDisponibles::encontrarPosicionInsertar(const std::string& numeroDeRegistro) {
    int i = 0;
    while (i < clavesUsadas && clave[i].getNumeroDeRegistro() < numeroDeRegistro) {
        ++i;
    }
    return i;
}

bool BNodoDisponibles::estaLleno() const {
    return clavesUsadas == (m - 1);
}

int BNodoDisponibles::buscarClave(const std::string& numeroDeRegistro) {
    int idx = 0;
    while (idx < clavesUsadas && clave[idx].getNumeroDeRegistro() < numeroDeRegistro) {
        ++idx;
    }
    return idx;
}

void BNodoDisponibles::eliminarClave(const std::string& numeroDeRegistro) {
    int idx = buscarClave(numeroDeRegistro);

    if (idx < clavesUsadas && clave[idx].getNumeroDeRegistro() == numeroDeRegistro) {
        if (esHoja()) {
            eliminarClaveHoja(idx);
        }
        else {
            eliminarClaveNoHoja(idx);
        }
    }
    else {
        if (esHoja()) {
            std::cout << "La clave " << numeroDeRegistro << " no existe en el �rbol\n";
            return;
        }

        bool flag = (idx == clavesUsadas);

        if (puntero[idx]->clavesUsadas < m / 2) {
            if (idx != 0 && puntero[idx - 1]->clavesUsadas >= m / 2) {
                tomarPrestadoDeAnterior(idx);
            }
            else if (idx != clavesUsadas && puntero[idx + 1]->clavesUsadas >= m / 2) {
                tomarPrestadoDeSiguiente(idx);
            }
            else {
                if (idx != clavesUsadas) {
                    fusionar(idx);
                }
                else {
                    fusionar(idx - 1);
                }
            }
        }

        if (flag && idx > clavesUsadas) {
            puntero[idx - 1]->eliminarClave(numeroDeRegistro);
        }
        else {
            puntero[idx]->eliminarClave(numeroDeRegistro);
        }
    }
}

void BNodoDisponibles::eliminarClaveHoja(int idx) {
    for (int i = idx + 1; i < clavesUsadas; ++i) {
        clave[i - 1] = clave[i];
    }
    clavesUsadas--;
}

void BNodoDisponibles::eliminarClaveNoHoja(int idx) {
    Avion k = clave[idx];

    if (puntero[idx]->clavesUsadas >= m / 2) {
        Avion predecesor = obtenerPredecesor(idx);
        clave[idx] = predecesor;
        puntero[idx]->eliminarClave(predecesor.getNumeroDeRegistro());
    }
    else if (puntero[idx + 1]->clavesUsadas >= m / 2) {
        Avion sucesor = obtenerSucesor(idx);
        clave[idx] = sucesor;
        puntero[idx + 1]->eliminarClave(sucesor.getNumeroDeRegistro());
    }
    else {
        fusionar(idx);
        puntero[idx]->eliminarClave(k.getNumeroDeRegistro());
    }
}

Avion BNodoDisponibles::obtenerPredecesor(int idx) {
    BNodoDisponibles* actual = puntero[idx];
    while (!actual->esHoja()) {
        actual = actual->puntero[actual->clavesUsadas];
    }
    return actual->clave[actual->clavesUsadas - 1];
}

Avion BNodoDisponibles::obtenerSucesor(int idx) {
    BNodoDisponibles* actual = puntero[idx + 1];
    while (!actual->esHoja()) {
        actual = actual->puntero[0];
    }
    return actual->clave[0];
}

void BNodoDisponibles::tomarPrestadoDeAnterior(int idx) {
    BNodoDisponibles* hijo = puntero[idx];
    BNodoDisponibles* hermano = puntero[idx - 1];

    for (int i = hijo->clavesUsadas - 1; i >= 0; --i) {
        hijo->clave[i + 1] = hijo->clave[i];
    }

    if (!hijo->esHoja()) {
        for (int i = hijo->clavesUsadas; i >= 0; --i) {
            hijo->puntero[i + 1] = hijo->puntero[i];
        }
    }

    hijo->clave[0] = clave[idx - 1];

    if (!hijo->esHoja()) {
        hijo->puntero[0] = hermano->puntero[hermano->clavesUsadas];
    }

    clave[idx - 1] = hermano->clave[hermano->clavesUsadas - 1];

    hijo->clavesUsadas += 1;
    hermano->clavesUsadas -= 1;
}

void BNodoDisponibles::tomarPrestadoDeSiguiente(int idx) {
    BNodoDisponibles* hijo = puntero[idx];
    BNodoDisponibles* hermano = puntero[idx + 1];

    hijo->clave[hijo->clavesUsadas] = clave[idx];

    if (!hijo->esHoja()) {
        hijo->puntero[hijo->clavesUsadas + 1] = hermano->puntero[0];
    }

    clave[idx] = hermano->clave[0];

    for (int i = 1; i < hermano->clavesUsadas; ++i) {
        hermano->clave[i - 1] = hermano->clave[i];
    }

    if (!hermano->esHoja()) {
        for (int i = 1; i <= hermano->clavesUsadas; ++i) {
            hermano->puntero[i - 1] = hermano->puntero[i];
        }
    }

    hijo->clavesUsadas += 1;
    hermano->clavesUsadas -= 1;
}

void BNodoDisponibles::fusionar(int idx) {
    BNodoDisponibles* hijo = puntero[idx];
    BNodoDisponibles* hermano = puntero[idx + 1];

    hijo->clave[m / 2 - 1] = clave[idx];

    for (int i = 0; i < hermano->clavesUsadas; ++i) {
        hijo->clave[i + m / 2] = hermano->clave[i];
    }

    if (!hijo->esHoja()) {
        for (int i = 0; i <= hermano->clavesUsadas; ++i) {
            hijo->puntero[i + m / 2] = hermano->puntero[i];
        }
    }

    for (int i = idx + 1; i < clavesUsadas; ++i) {
        clave[i - 1] = clave[i];
    }

    for (int i = idx + 2; i <= clavesUsadas; ++i) {
        puntero[i - 1] = puntero[i];
    }

    hijo->clavesUsadas += hermano->clavesUsadas + 1;
    clavesUsadas--;

    delete hermano;
}






class ArbolBAvionesDisponibles {
public:
    ArbolBAvionesDisponibles(int m);   
    ~ArbolBAvionesDisponibles();     

    void insertar(const Avion& nuevoAvion);
    void eliminar(const std::string& numeroDeRegistro);
    void graficar(const std::string& filename);
    void imprimirClavesOrdenadas();
    Avion* buscar(const std::string& numeroDeRegistro);

private:
    BNodoDisponibles* raiz;
    int m; 

    void insertarNoLleno(BNodoDisponibles* nodo, const Avion& nuevoAvion);
    void destruirNodo(BNodoDisponibles* nodo);
    void graficarNodo(std::stringstream& ss, BNodoDisponibles* nodo, int& nullCount);
    void imprimirClavesOrdenadas(BNodoDisponibles* nodo);
    Avion* buscarEnNodo(BNodoDisponibles* nodo, const std::string& numeroDeRegistro);

    
    std::string obtenerDot();

    void ejecutarComando(const std::string& comando);
};

ArbolBAvionesDisponibles::ArbolBAvionesDisponibles(int m) : raiz(new BNodoDisponibles(m)), m(m) {}

ArbolBAvionesDisponibles::~ArbolBAvionesDisponibles() {
    destruirNodo(raiz);
}

Avion* ArbolBAvionesDisponibles::buscar(const std::string& numeroDeRegistro) {
    return buscarEnNodo(raiz, numeroDeRegistro);
}

Avion* ArbolBAvionesDisponibles::buscarEnNodo(BNodoDisponibles* nodo, const std::string& numeroDeRegistro) {
    if (!nodo) return nullptr;

    int idx = nodo->buscarClave(numeroDeRegistro);
    if (idx < nodo->clavesUsadas && nodo->clave[idx].getNumeroDeRegistro() == numeroDeRegistro) {
        return &nodo->clave[idx];
    }

    if (nodo->esHoja()) {
        return nullptr;
    }
    else {
        return buscarEnNodo(nodo->puntero[idx], numeroDeRegistro);
    }
}
void ArbolBAvionesDisponibles::destruirNodo(BNodoDisponibles* nodo) {
    if (nodo) {
        if (!nodo->esHoja()) {
            for (int i = 0; i <= nodo->clavesUsadas; ++i) {
                destruirNodo(nodo->puntero[i]);
            }
        }
        delete nodo;
    }
}

void ArbolBAvionesDisponibles::insertar(const Avion& nuevoAvion) {
    if (raiz->estaLleno()) {
        BNodoDisponibles* nuevaRaiz = new BNodoDisponibles(m);
        nuevaRaiz->hoja = false;
        nuevaRaiz->puntero[0] = raiz;
        Avion mediana;
        BNodoDisponibles* nuevoNodo = raiz->dividirNodo(mediana);
        nuevaRaiz->clave[0] = mediana;
        nuevaRaiz->puntero[1] = nuevoNodo;
        nuevaRaiz->clavesUsadas = 1;
        raiz = nuevaRaiz;
    }
    insertarNoLleno(raiz, nuevoAvion);
}

void ArbolBAvionesDisponibles::insertarNoLleno(BNodoDisponibles* nodo, const Avion& nuevoAvion) {
    if (nodo == nullptr) {
        std::cerr << "Error: nodo es nullptr en insertarNoLleno." << std::endl;
        return;
    }

    if (nodo->esHoja()) {
        nodo->insertarClave(nuevoAvion);
    }
    else {
        int i = nodo->encontrarPosicionInsertar(nuevoAvion.getNumeroDeRegistro());
        if (nodo->puntero[i] == nullptr) {
            std::cerr << "Error: nodo->puntero[" << i << "] es nullptr en insertarNoLleno." << std::endl;
            return;
        }

        if (nodo->puntero[i]->estaLleno()) {
            Avion mediana;
            BNodoDisponibles* nuevoNodo = nodo->puntero[i]->dividirNodo(mediana);
            nodo->insertarClave(mediana);

            for (int j = nodo->clavesUsadas; j > i + 1; --j) {
                nodo->puntero[j] = nodo->puntero[j - 1];
            }

            nodo->puntero[i + 1] = nuevoNodo;

            if (nuevoAvion.getNumeroDeRegistro() > mediana.getNumeroDeRegistro()) {
                ++i;
            }
        }

        insertarNoLleno(nodo->puntero[i], nuevoAvion);
    }
}



void ArbolBAvionesDisponibles::eliminar(const std::string& numeroDeRegistro) {
    if (!raiz) {
        std::cout << "El �rbol est� vac�o\n";
        return;
    }

    raiz->eliminarClave(numeroDeRegistro);

    if (raiz->clavesUsadas == 0) {
        BNodoDisponibles* temp = raiz;
        if (raiz->esHoja()) {
            raiz = nullptr;
        }
        else {
            raiz = raiz->puntero[0];
        }
        delete temp;
    }
}

void ArbolBAvionesDisponibles::graficar(const std::string& filename) {
    std::ofstream archivo(filename + ".dot");
    archivo << obtenerDot();
    archivo.close();

    std::string comandoDot = "dot -Tpng " + filename + ".dot -o " + filename + ".png";
    std::system((comandoDot + " > nul 2>&1").c_str());

    std::string comandoAbrir = "start " + filename + ".png";
    std::system((comandoAbrir + " > nul 2>&1").c_str());
}



std::string ArbolBAvionesDisponibles::obtenerDot() {
    std::stringstream ss;
    ss << "digraph ArbolBAvionesDisponibles {\n";
    ss << "bgcolor=lightblue;\n";
    ss << "Node [shape=circle style=filled fillcolor=lightblue fontcolor=black];\n";
    if (raiz) {
        int nullCount = 0;
        graficarNodo(ss, raiz, nullCount);
    }
    ss << "}\n";
    return ss.str();
}



void ArbolBAvionesDisponibles::graficarNodo(std::stringstream& ss, BNodoDisponibles* nodo, int& nullCount) {
    if (!nodo) return;

    ss << "Node" << nodo << " [label=\"";
    for (int i = 0; i < nodo->clavesUsadas; ++i) {
        if (i > 0) ss << " | ";
        ss << nodo->clave[i].getNumeroDeRegistro();
    }
    ss << "\"];\n";

    if (!nodo->esHoja()) {
        for (int i = 0; i <= nodo->clavesUsadas; ++i) {
            if (nodo->puntero[i]) {
                ss << "Node" << nodo << " -> Node" << nodo->puntero[i] << ";\n";
                graficarNodo(ss, nodo->puntero[i], nullCount);
            }
            else {
                ss << "Node" << nodo << " -> null" << nullCount << ";\n";
                ss << "null" << nullCount << " [shape=point];\n";
                nullCount++;
            }
        }
    }
}





void ArbolBAvionesDisponibles::ejecutarComando(const std::string& comando) {
    system(comando.c_str());
}

void ArbolBAvionesDisponibles::imprimirClavesOrdenadas() {
    if (raiz) {
        imprimirClavesOrdenadas(raiz);
    }
    std::cout << std::endl;
}

void ArbolBAvionesDisponibles::imprimirClavesOrdenadas(BNodoDisponibles* nodo) {
    if (!nodo) return;

    int i;
    for (i = 0; i < nodo->clavesUsadas; i++) {
        if (!nodo->esHoja()) {
            imprimirClavesOrdenadas(nodo->puntero[i]);
        }
        std::cout << nodo->clave[i].getNumeroDeRegistro() << " ";
    }
    if (!nodo->esHoja()) {
        imprimirClavesOrdenadas(nodo->puntero[i]);
    }
}


template <typename T>
class NodoLista {
private:
    std::string clave;
    T valor;
    NodoLista* siguiente;

public:
    
    NodoLista(const std::string& key, T val) : clave(key), valor(val), siguiente(nullptr) {}

  
    std::string getClave() const {
        return clave;
    }

    T getValor() const {
        return valor;
    }

    NodoLista* getSiguiente() const {
        return siguiente;
    }

    
    void setClave(const std::string& key) {
        clave = key;
    }

    void setValor(T val) {
        valor = val;
    }

    void setSiguiente(NodoLista* sig) {
        siguiente = sig;
    }
};

template <typename T>
class ListaEnlazada {
private:
    NodoLista<T>* cabeza;

public:
   
    ListaEnlazada() : cabeza(nullptr) {}

    ~ListaEnlazada() {
        while (cabeza != nullptr) {
            NodoLista<T>* temp = cabeza;
            cabeza = cabeza->getSiguiente();
            delete temp;
        }
    }

    void agregar(const std::string& clave, T valor) {
        NodoLista<T>* nuevoNodo = new NodoLista<T>(clave, valor);
        nuevoNodo->setSiguiente(cabeza);
        cabeza = nuevoNodo;
    }

    T obtener(const std::string& clave) const {
        NodoLista<T>* actual = cabeza;
        while (actual != nullptr) {
            if (actual->getClave() == clave) {
                return actual->getValor();
            }
            actual = actual->getSiguiente();
        }
        return std::numeric_limits<T>::max(); 
    }

    void actualizar(const std::string& clave, T valor) {
        NodoLista<T>* actual = cabeza;
        while (actual != nullptr) {
            if (actual->getClave() == clave) {
                actual->setValor(valor);
                return;
            }
            actual = actual->getSiguiente();
        }
   
        agregar(clave, valor);
    }

    bool existe(const std::string& clave) const {
        NodoLista<T>* actual = cabeza;
        while (actual != nullptr) {
            if (actual->getClave() == clave) {
                return true;
            }
            actual = actual->getSiguiente();
        }
        return false;
    }
};


class NodoCola {
private:
    std::string vertice;
    double distancia;
    NodoCola* siguiente;

public:
   
    NodoCola(const std::string& vert, double dist) : vertice(vert), distancia(dist), siguiente(nullptr) {}

   
    std::string getVertice() const {
        return vertice;
    }

    double getDistancia() const {
        return distancia;
    }

    NodoCola* getSiguiente() const {
        return siguiente;
    }

    
    void setVertice(const std::string& vert) {
        vertice = vert;
    }

    void setDistancia(double dist) {
        distancia = dist;
    }

    void setSiguiente(NodoCola* sig) {
        siguiente = sig;
    }
};






class ColaPrioridad {
private:
    NodoCola* frente;

public:
    
    ColaPrioridad() : frente(nullptr) {}

   
    ~ColaPrioridad() {
        while (frente != nullptr) {
            NodoCola* temp = frente;
            frente = frente->getSiguiente();
            delete temp;
        }
    }

    
    void insertar(const std::string& vertice, double distancia) {
        NodoCola* nuevoNodo = new NodoCola(vertice, distancia);
        if (frente == nullptr || distancia < frente->getDistancia()) {
            nuevoNodo->setSiguiente(frente);
            frente = nuevoNodo;
        }
        else {
            NodoCola* actual = frente;
            while (actual->getSiguiente() != nullptr && actual->getSiguiente()->getDistancia() <= distancia) {
                actual = actual->getSiguiente();
            }
            nuevoNodo->setSiguiente(actual->getSiguiente());
            actual->setSiguiente(nuevoNodo);
        }
    }

    
    std::string extraerMin() {
        if (frente == nullptr) {
            return "";
        }
        NodoCola* temp = frente;
        frente = frente->getSiguiente();
        std::string minVertice = temp->getVertice();
        delete temp;
        return minVertice;
    }

    
    bool estaVacia() const {
        return frente == nullptr;
    }
};


template <typename T>
class Pila {
private:
    NodoLista<T>* cima;

public:
    
    Pila() : cima(nullptr) {}

    
    ~Pila() {
        while (cima != nullptr) {
            NodoLista<T>* temp = cima;
            cima = cima->getSiguiente();
            delete temp;
        }
    }

    
    void push(const std::string& clave) {
        NodoLista<T>* nuevoNodo = new NodoLista<T>(clave, T{});  
        nuevoNodo->setSiguiente(cima);
        cima = nuevoNodo;
    }

    
    std::string pop() {
        if (cima == nullptr) {
            return "";
        }
        NodoLista<T>* temp = cima;
        cima = cima->getSiguiente();
        std::string clave = temp->getClave();
        delete temp;
        return clave;
    }

    
    bool estaVacia() const {
        return cima == nullptr;
    }

    
    std::string top() const {
        if (cima == nullptr) {
            return "";
        }
        return cima->getClave();
    }
};




struct Ruta {
    std::string destino;
    double distancia;
    Ruta* siguiente;

    Ruta(const std::string& dest, double dist) : destino(dest), distancia(dist), siguiente(nullptr) {}
};

struct Vertice {
    std::string origen;
    Ruta* rutas;
    Vertice* siguiente;

    Vertice(const std::string& orig) : origen(orig), rutas(nullptr), siguiente(nullptr) {}
};


class ListaDeAdyacenciaRutas {
private:
    Vertice* vertices; 

public:
    
    ListaDeAdyacenciaRutas() : vertices(nullptr) {}

    
    ~ListaDeAdyacenciaRutas() {
        while (vertices != nullptr) {
            Vertice* tempVertice = vertices;
            vertices = vertices->siguiente;
            while (tempVertice->rutas != nullptr) {
                Ruta* tempRuta = tempVertice->rutas;
                tempVertice->rutas = tempVertice->rutas->siguiente;
                delete tempRuta;
            }
            delete tempVertice;
        }
    }

    void agregarVertice(const std::string& origen) {
        if (encontrarVertice(origen) != nullptr) {
            return;
        }

        Vertice* nuevoVertice = new Vertice(origen);
        nuevoVertice->siguiente = vertices;
        vertices = nuevoVertice;
    }

    Vertice* encontrarVertice(const std::string& origen) {
        Vertice* actual = vertices;
        while (actual != nullptr) {
            if (actual->origen == origen) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    void agregarRuta(const std::string& origen, const std::string& destino, double distancia) {
        Vertice* verticeOrigen = encontrarVertice(origen);
        if (verticeOrigen == nullptr) {
            std::cerr << "Vértice " << origen << " no encontrado." << std::endl;
            return;
        }

        Ruta* nuevaRuta = new Ruta(destino, distancia);
        nuevaRuta->siguiente = verticeOrigen->rutas;
        verticeOrigen->rutas = nuevaRuta;
    }

    void GenerarGraficoRutas() {
        Vertice* verticeActual = vertices;
        while (verticeActual != nullptr) {
            std::cout << "Vertice " << verticeActual->origen << ":";
            Ruta* rutaActual = verticeActual->rutas;
            while (rutaActual != nullptr) {
                std::cout << " -> [" << rutaActual->destino << ", " << rutaActual->distancia << " km]";
                rutaActual = rutaActual->siguiente;
            }
            std::cout << std::endl;
            verticeActual = verticeActual->siguiente;
        }
    }

    
    void GrafoDirigidoParaRutas() {
        std::string nombreArchivo = "GrafoDirigido_rutas";
        if (vertices == nullptr) {
            std::cerr << "La lista de adyacencia está vacía, no hay datos para generar el reporte." << std::endl;
            return;
        }

        std::string dotFile = nombreArchivo + ".dot";
        std::string pngFile = nombreArchivo + ".png";
        std::ofstream file(dotFile);

        if (!file) {
            std::cerr << "Error al abrir el archivo: " << dotFile << std::endl;
            return;
        }

        file << "digraph G {\n";
        file << "rankdir=TB;\n";
        file << "Node [shape=circle, style=filled, fillcolor=\"#000080\", fontcolor=\"white\"];\n";
        file << "bgcolor=\"lightblue\";\n";
        file << "edge [color=\"#000080\"];\n"; 
        file << "label=\"Grafo Dirigido de Rutas\";\n";


        Vertice* verticeActual = vertices;
        while (verticeActual != nullptr) {
            Ruta* rutaActual = verticeActual->rutas;
            while (rutaActual != nullptr) {
                file << "\"" << verticeActual->origen << "\" -> \"" << rutaActual->destino
                    << "\" [label=\"" << rutaActual->distancia << " km\"];\n";
                rutaActual = rutaActual->siguiente;
            }
            verticeActual = verticeActual->siguiente;
        }
        file << "}\n";
        file.close();

        
        std::string command = "dot -Tpng " + dotFile + " -o " + pngFile;
        if (system(command.c_str()) != 0) {
            std::cerr << "Error al ejecutar Graphviz." << std::endl;
            return;
        }

        
        command = "start " + pngFile;
        if (system(command.c_str()) != 0) {
            std::cerr << "Error al abrir la imagen." << std::endl;
            return;
        }
    }

    
    void encontrarRutaMasCorta(const std::string& origen, const std::string& destino) {
        ListaEnlazada<double> distancias;
        ListaEnlazada<std::string> predecesores;
        ColaPrioridad cola;

        Vertice* verticeActual = vertices;
        while (verticeActual != nullptr) {
            double distanciaInicial = (verticeActual->origen == origen) ? 0 : std::numeric_limits<double>::infinity();
            distancias.agregar(verticeActual->origen, distanciaInicial);
            cola.insertar(verticeActual->origen, distanciaInicial);
            verticeActual = verticeActual->siguiente;
        }

        while (!cola.estaVacia()) {
            std::string vertice = cola.extraerMin();
            Vertice* actual = encontrarVertice(vertice);

            Ruta* rutaActual = actual->rutas;
            while (rutaActual != nullptr) {
                double nuevaDistancia = distancias.obtener(vertice) + rutaActual->distancia;
                if (nuevaDistancia < distancias.obtener(rutaActual->destino)) {
                    distancias.actualizar(rutaActual->destino, nuevaDistancia);
                    predecesores.actualizar(rutaActual->destino, vertice);
                    cola.insertar(rutaActual->destino, nuevaDistancia);
                }
                rutaActual = rutaActual->siguiente;
            }
        }

        if (distancias.obtener(destino) == std::numeric_limits<double>::infinity()) {
            std::cout << "No hay camino desde " << origen << " hacia " << destino << std::endl;
        }
        else {
            std::string verticeActualStr = destino;
            Pila<std::string> ruta;
            while (verticeActualStr != origen) {
                ruta.push(verticeActualStr);
                verticeActualStr = predecesores.obtener(verticeActualStr);
            }
            ruta.push(origen);

            double distanciaTotal = distancias.obtener(destino);

            std::cout << "La ruta mas corta desde " << origen << " para llegar hasta " << destino << " es:\n";
            while (!ruta.estaVacia()) {
                std::cout << ruta.pop();
                if (!ruta.estaVacia()) {
                    std::cout << " -> ";
                }
            }
            std::cout << "\nDistancia mas corta a recorrer: " << distanciaTotal << " km\n";
        }
    }
};





class Node {

public:
    string row;
    string col;
    string val;
    Node* up;
    Node* down;
    Node* left;
    Node* right;

    Node(string r = "", string c = "", string v = "") : row(r), col(c), val(v), up(nullptr), down(nullptr), left(nullptr), right(nullptr) {}
};

class MatrizDispersaVuelosYDestinos {
public:
  
    MatrizDispersaVuelosYDestinos();

    
    ~MatrizDispersaVuelosYDestinos();

    
    Node* createNode(string row, string col, string val);

    
    void printCityAndFlightByPilotID(const string& pilotID);

    void ObtenerGraphviz();

    void deletePilotByID(const string& pilotID);

private:
    Node* head;

    
    Node* createVertHead(string row);

    
    Node* createHorzHead(string col);

   
    Node* searchHorzHead(string col);

    
    Node* searchVertHead(string row);

  
    void deleteAllNodes(Node* Node);

    string replaceSpaces(const string& str);
    
};


MatrizDispersaVuelosYDestinos::MatrizDispersaVuelosYDestinos() {
    head = new Node();
}

MatrizDispersaVuelosYDestinos::~MatrizDispersaVuelosYDestinos() {
    
}

void MatrizDispersaVuelosYDestinos::deleteAllNodes(Node* Node) {
    if (Node == nullptr) return;
    deleteAllNodes(Node->right);
    deleteAllNodes(Node->down);
    delete Node;
}


string MatrizDispersaVuelosYDestinos::replaceSpaces(const string& str) {
    string result = str;
    replace(result.begin(), result.end(), ' ', '_');
    return result;
}


void MatrizDispersaVuelosYDestinos::deletePilotByID(const string& pilotID) {
    Node* rowHead = head->down;
    Node* targetNode = nullptr;

   
    while (rowHead != nullptr && targetNode == nullptr) {
        Node* colNode = rowHead->right;
        while (colNode != nullptr) {
            if (colNode->val == pilotID) {
                targetNode = colNode;
                break;
            }
            colNode = colNode->right;
        }
        rowHead = rowHead->down;
    }

    
    if (targetNode == nullptr) {
        cout << "Pilot ID not found." << endl;
        return;
    }

    
    if (targetNode->left != nullptr) {
        targetNode->left->right = targetNode->right;
    }
    if (targetNode->right != nullptr) {
        targetNode->right->left = targetNode->left;
    }
    if (targetNode->up != nullptr) {
        targetNode->up->down = targetNode->down;
    }
    if (targetNode->down != nullptr) {
        targetNode->down->up = targetNode->up;
    }

    
    Node* colHead = searchHorzHead(targetNode->col);
    if (colHead != nullptr && colHead->down == nullptr) {
        if (colHead->left != nullptr) {
            colHead->left->right = colHead->right;
        }
        if (colHead->right != nullptr) {
            colHead->right->left = colHead->left;
        }
        delete colHead;
    }

   
    rowHead = searchVertHead(targetNode->row);
    if (rowHead != nullptr && rowHead->right == nullptr) {
        if (rowHead->up != nullptr) {
            rowHead->up->down = rowHead->down;
        }
        if (rowHead->down != nullptr) {
            rowHead->down->up = rowHead->up;
        }
        delete rowHead;
    }

   
    delete targetNode;
    cout << "Pilot ID '" << pilotID << "' removed successfully." << endl;
}





Node* MatrizDispersaVuelosYDestinos::createNode(string row, string col, string val) {
    Node* newNode = new Node(row, replaceSpaces(col), val);

    Node* temp = createVertHead(row);
    Node* temp2 = createHorzHead(replaceSpaces(col));

    std::cout << temp->col;
    std::cout << temp2->col;

    Node* temp3 = temp->right;
    Node* temp4 = temp2->down;

    while (temp3 != nullptr) {
        if (temp3->col > col) {
            break;
        }
        temp = temp->right;
        temp3 = temp3->right;
    }

    while (temp4 != nullptr) {
        if (temp4->row > row) {
            break;
        }
        temp2 = temp2->down;
        temp4 = temp4->down;
    }

    newNode->right = temp3;
    newNode->left = temp;
    newNode->up = temp2;
    newNode->down = temp4;

    if (temp3 != nullptr) {
        temp3->left = newNode;
    }
    if (temp4 != nullptr) {
        temp4->up = newNode;
    }
    temp->right = newNode;
    temp2->down = newNode;

    return newNode;
}

Node* MatrizDispersaVuelosYDestinos::searchHorzHead(string col) {
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->col == col) {
            return temp;
        }
        temp = temp->right;
    }
    return nullptr;
}

Node* MatrizDispersaVuelosYDestinos::createHorzHead(string col) {
    Node* newNode = searchHorzHead(col);
    if (newNode != nullptr) {
        return newNode;
    }

    newNode = new Node("", col, "");

    Node* temp = head;

    while (temp->right != nullptr) {
        if (temp->right->col > col) {
            break;
        }
        temp = temp->right;
    }
    newNode->right = temp->right;
    if (temp->right != nullptr) {
        temp->right->left = newNode;
    }
    temp->right = newNode;
    newNode->left = temp;

    return newNode;
}


Node* MatrizDispersaVuelosYDestinos::searchVertHead(string row) {
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->row == row) {
            return temp;
        }
        temp = temp->down;
    }
    return nullptr;
}


Node* MatrizDispersaVuelosYDestinos::createVertHead(string row) {
    Node* newNode = searchVertHead(row);
    if (newNode != nullptr) {
        return newNode;
    }

    newNode = new Node(row, "", "");

    Node* temp = head;
   
    while (temp->down != nullptr) {
        if (temp->down->row > row) {
            break;
        }
        temp = temp->down;
    }
    newNode->down = temp->down;
    if (temp->down != nullptr) {
        temp->down->up = newNode;
    }
    temp->down = newNode;
    newNode->up = temp;

    return newNode;
}



void MatrizDispersaVuelosYDestinos::printCityAndFlightByPilotID(const string& pilotID) {
    Node* rowHead = head->down;
    while (rowHead != nullptr) {
        Node* colNode = rowHead->right;
        while (colNode != nullptr) {
            if (colNode->val == pilotID) {
                cout << "Flight Code: " << colNode->row << ", City: " << colNode->col << endl;
                return;
            }
            colNode = colNode->right;
        }
        rowHead = rowHead->down;
    }
    cout << "Pilot ID not found." << endl;
}

void MatrizDispersaVuelosYDestinos::ObtenerGraphviz() {
    FILE* fp;
    errno_t err = fopen_s(&fp, "MatrizDispersa.dot", "w+");
    if (err != 0) {
        fprintf(stderr, "Error al abrir el archivo 'MatrizDispersa.dot'\n");
        return;
    }

    fprintf(fp, "digraph Sparce_Matrix {\n");
    fprintf(fp, "Node [shape=box];\n");

  
    fprintf(fp, "Mt [label=\"VUELOS**DESTINOS\", width=1.5, style=filled, fillcolor=blue,fontcolor=white, group=1];\n");

   
    fprintf(fp, "e0 [shape=point, width=0];\n");
    fprintf(fp, "e1 [shape=point, width=0];\n");


    Node* temp = head->down;
    int rowLevel = 1;
    while (temp != nullptr) {
        fprintf(fp, "Node%s [label=\"%s\", width=1.5, style=filled, fillcolor=black,fontcolor=white ,group=1];\n",
            replaceSpaces(temp->row).c_str(), replaceSpaces(temp->row).c_str());
        if (temp == head->down) {
            fprintf(fp, "Mt -> Node%s;\n", replaceSpaces(temp->row).c_str());
        }
        if (temp->down != nullptr) {
            fprintf(fp, "Node%s -> Node%s;\n",
                replaceSpaces(temp->row).c_str(), replaceSpaces(temp->down->row).c_str());
        }
        temp = temp->down;
        rowLevel++;
    }


    temp = head->right;
    int colCount = 2; 
    string rankCol = "{ rank=same; Mt; ";
    while (temp != nullptr) {
        rankCol += "Node" + replaceSpaces(temp->col) + "; ";
        fprintf(fp, "Node%s [label=\"%s\", width=1.5, style=filled, fillcolor=black,fontcolor=white, group=%d];\n",
            replaceSpaces(temp->col).c_str(), replaceSpaces(temp->col).c_str(), colCount);
        if (temp == head->right) {
            fprintf(fp, "Mt -> Node%s;\n", replaceSpaces(temp->col).c_str());
        }
        if (temp->right != nullptr) {
            fprintf(fp, "Node%s -> Node%s;\n",
                replaceSpaces(temp->col).c_str(), replaceSpaces(temp->right->col).c_str());
        }
        temp = temp->right;
        colCount++;
    }
    rankCol += "};\n";
    fprintf(fp, "%s", rankCol.c_str());

   
    temp = head->down;
    while (temp != nullptr) {
        Node* temp2 = temp->right;
        string rankRow = "{ rank=same; Node" + replaceSpaces(temp->row) + "; ";
        while (temp2 != nullptr) {
            int colGroup = 2; 
            Node* header = head->right;
            while (header != nullptr && replaceSpaces(header->col) != replaceSpaces(temp2->col)) {
                header = header->right;
                colGroup++;
            }
            rankRow += "Node" + replaceSpaces(temp2->row) + replaceSpaces(temp2->col) + "; ";
            fprintf(fp, "Node%s%s [label=\"%s\", width=1.5, group=%d];\n",
                replaceSpaces(temp2->row).c_str(), replaceSpaces(temp2->col).c_str(),
                replaceSpaces(temp2->val).c_str(), colGroup);

            

            if (temp2->right != nullptr) {
                fprintf(fp, "Node%s%s -> Node%s%s;\n",
                    replaceSpaces(temp2->row).c_str(), replaceSpaces(temp2->col).c_str(),
                    replaceSpaces(temp2->right->row).c_str(), replaceSpaces(temp2->right->col).c_str());
                fprintf(fp, "Node%s%s -> Node%s%s;\n",
                    replaceSpaces(temp2->right->row).c_str(), replaceSpaces(temp2->right->col).c_str(),
                    replaceSpaces(temp2->row).c_str(), replaceSpaces(temp2->col).c_str());
            }

            if (temp2->left != nullptr) {
                fprintf(fp, "Node%s%s -> Node%s%s;\n",
                    replaceSpaces(temp2->row).c_str(), replaceSpaces(temp2->col).c_str(),
                    replaceSpaces(temp2->left->row).c_str(), replaceSpaces(temp2->left->col).c_str());
                fprintf(fp, "Node%s%s -> Node%s%s;\n",
                    replaceSpaces(temp2->left->row).c_str(), replaceSpaces(temp2->left->col).c_str(),
                    replaceSpaces(temp2->row).c_str(), replaceSpaces(temp2->col).c_str());
            }

           
            if (temp2->up != nullptr) {
                fprintf(fp, "Node%s%s -> Node%s%s;\n",
                    replaceSpaces(temp2->up->row).c_str(), replaceSpaces(temp2->up->col).c_str(),
                    replaceSpaces(temp2->row).c_str(), replaceSpaces(temp2->col).c_str());
                fprintf(fp, "Node%s%s -> Node%s%s;\n",
                    replaceSpaces(temp2->row).c_str(), replaceSpaces(temp2->col).c_str(),
                    replaceSpaces(temp2->up->row).c_str(), replaceSpaces(temp2->up->col).c_str());
            }
            temp2 = temp2->right;
        }
        rankRow += "};\n";
        fprintf(fp, "%s", rankRow.c_str());
        temp = temp->down;
    }



    fprintf(fp, "}\n");

    fclose(fp);
    system("dot -Tpng MatrizDispersa.dot -o MatrizDispersa.png");
    system("start MatrizDispersa.png");
}






void cargarPilotos(const std::string& filename,
    ArbolBinarioDeBusquedaPilotos& arbolPilotos,
    TablaHashPilotos& tablaHashPilotos,
    MatrizDispersaVuelosYDestinos& matrizDispersa,
    const std::unordered_map<std::string, Avion>& avionesPorVuelo) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "No se pudo abrir el archivo JSON: " << filename << std::endl;
        return;
    }

    json j;
    try {
        ifs >> j;
    }
    catch (const std::exception& e) {
        std::cerr << "Error al parsear el JSON: " << e.what() << std::endl;
        return;
    }

    for (const auto& pilotoJson : j) {
        try {
            if (!pilotoJson.contains("nombre") || !pilotoJson.contains("nacionalidad") ||
                !pilotoJson.contains("numero_de_id") || !pilotoJson.contains("vuelo") ||
                !pilotoJson.contains("horas_de_vuelo") || !pilotoJson.contains("tipo_de_licencia")) {
                std::cerr << "Piloto JSON malformado" << std::endl;
                continue;
            }

            std::string nombre = pilotoJson.at("nombre").get<std::string>();
            std::string nacionalidad = pilotoJson.at("nacionalidad").get<std::string>();
            std::string numero_de_id = pilotoJson.at("numero_de_id").get<std::string>();
            std::string vuelo = pilotoJson.at("vuelo").get<std::string>();
            int horas_de_vuelo = pilotoJson.at("horas_de_vuelo").get<int>();
            std::string tipo_de_licencia = pilotoJson.at("tipo_de_licencia").get<std::string>();

           
            Piloto piloto(nombre, nacionalidad, numero_de_id, vuelo, horas_de_vuelo, tipo_de_licencia);

            
            arbolPilotos.insertar(piloto);
            tablaHashPilotos.insertar(piloto);

            
            auto it = avionesPorVuelo.find(vuelo);
            if (it != avionesPorVuelo.end()) {
                const Avion& avion = it->second;
                std::string row = vuelo; 
                std::string col = avion.getCiudadDestino(); 
                std::string val = numero_de_id; 

                matrizDispersa.createNode(row, col, val);
            }
            else {
                std::cerr << "No se encontró un avión para el vuelo: " << vuelo << std::endl;
            }

            std::cout << "Piloto cargado: " << piloto << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error al procesar piloto: " << e.what() << std::endl;
        }
    }

    std::cout << "Pilotos cargados exitosamente." << std::endl;
    ifs.close();
}















void cargarRutas(const std::string& filename, ListaDeAdyacenciaRutas& grafo) {
    std::ifstream archivo(filename);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
        return;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string origen, destino;
        double distancia;

        if (std::getline(ss, origen, '/') &&
            std::getline(ss, destino, '/') &&
            (ss >> distancia)) {
            grafo.agregarVertice(origen);
            grafo.agregarVertice(destino);
            grafo.agregarRuta(origen, destino, distancia);
        }
        else {
            std::cerr << "Formato inválido en la línea: " << linea << std::endl;
        }
    }

    archivo.close();
}


void cargarAvionesDesdeJSON(const std::string& filename,
    ArbolBAvionesDisponibles& arbolB,
    ListaCircularlistaMantenimiento& listaMantenimiento,
    std::unordered_map<std::string, Avion>& avionesPorVuelo) {
    std::ifstream archivo(filename);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << filename << std::endl;
        return;
    }

    json j;
    archivo >> j;
    archivo.close();

    std::vector<Avion> avionesCargados;

    for (const auto& item : j) {
        Avion avion(
            item["vuelo"].get<std::string>(),
            item["numero_de_registro"].get<std::string>(),
            item["modelo"].get<std::string>(),
            item["capacidad"].get<int>(),
            item["aerolinea"].get<std::string>(),
            item["ciudad_destino"].get<std::string>(),
            item["estado"].get<std::string>()
        );

        avionesCargados.push_back(avion);
        avionesPorVuelo[avion.getVuelo()] = avion;

        if (avion.estado == "Disponible") {
            arbolB.insertar(avion);
        }
        else if (avion.estado == "Mantenimiento") {
            listaMantenimiento.agregarAvionEnMantenimiento(avion);
        }
        else {
            std::cerr << "Estado desconocido para el avion con numero de registro: " << avion.numero_de_registro << std::endl;
        }
    }

    std::cout << "Aviones cargados:" << std::endl;
    for (const auto& avion : avionesCargados) {
        avion.imprimir();
    }
}




void CargarComandos(const std::string& rutaArchivo, ArbolBAvionesDisponibles& arbolAviones, ListaCircularlistaMantenimiento& listaMantenimiento, ArbolBinarioDeBusquedaPilotos& arbolPilotos, TablaHashPilotos& tablaPilotos, MatrizDispersaVuelosYDestinos& matrizDispersa) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << rutaArchivo << std::endl;
        return;
    }

    std::string linea;

    while (std::getline(archivo, linea)) {
        if (linea.find("MantenimientoAviones,Ingreso,") != std::string::npos) {
            std::string numero_de_registro = linea.substr(linea.find_last_of(',') + 1);
            if (numero_de_registro.back() == ';') {
                numero_de_registro.pop_back();
            }

            Avion* avion = arbolAviones.buscar(numero_de_registro);
            if (avion) {
                arbolAviones.eliminar(numero_de_registro);
                avion->estado = "Mantenimiento";
                listaMantenimiento.agregarAvionEnMantenimiento(*avion);
                std::cout << "El avión con número de registro " << numero_de_registro << " fue movido a mantenimiento." << std::endl;
            }
            else {
                std::cerr << "El avión con número de registro " << numero_de_registro << " no se encontró en la lista de aviones disponibles." << std::endl;
            }
        }
        else if (linea.find("MantenimientoAviones,Salida,") != std::string::npos) {
            std::string numero_de_registro = linea.substr(linea.find_last_of(',') + 1);
            if (numero_de_registro.back() == ';') {
                numero_de_registro.pop_back();
            }

            Avion avion;
            if (listaMantenimiento.eliminarAvionPorRegistro(numero_de_registro, avion)) {
                avion.estado = "Disponible";
                arbolAviones.insertar(avion);
                std::cout << "El avión con número de registro " << numero_de_registro << " ha salido de mantenimiento." << std::endl;
            }
            else {
                std::cerr << "El avión con número de registro " << numero_de_registro << " no se encontró en la lista de aviones en mantenimiento." << std::endl;
            }
        }
        else if (linea.find("DarDeBaja(") != std::string::npos) {
            std::string id_piloto = linea.substr(linea.find('(') + 1, linea.find(')') - linea.find('(') - 1);
            if (id_piloto.back() == ';') {
                id_piloto.pop_back();
            }

            try {
                arbolPilotos.eliminar(id_piloto);
                tablaPilotos.eliminar(id_piloto);
                matrizDispersa.deletePilotByID(id_piloto);
                std::cout << "El piloto con ID " << id_piloto << " ha sido dado de baja." << std::endl;
            }
            catch (const std::runtime_error& e) {
                std::cerr << "Error al dar de baja al piloto con ID " << id_piloto << ": " << e.what() << std::endl;
            }
        }
    }

    archivo.close();
}




int main() {
    int opcion;

    ArbolBinarioDeBusquedaPilotos arbolPilotos;
    ListaDeAdyacenciaRutas grafoRutas;
    ArbolBAvionesDisponibles arbolAviones(5);
    ListaCircularlistaMantenimiento listaMantenimiento;
    TablaHashPilotos TablaHashPilotos;
    MatrizDispersaVuelosYDestinos matrizDispersa;
    std::unordered_map<std::string, Avion> avionesPorVuelo;

    do {
        std::cout << std::endl;
        std::cout << "========== M E N U - P R I N C I P A L ===========" << std::endl;
        std::cout << std::endl;
        std::cout << "1- Cargar Aviones" << std::endl;
        std::cout << "2- Cargar Pilotos" << std::endl;
        std::cout << "3- Cargar Rutas" << std::endl;
        std::cout << "4- Cargar Movimientos" << std::endl;
        std::cout << "5- Consultar horas de vuelo (pilotos)" << std::endl;
        std::cout << "6- Recomendar Ruta" << std::endl;
        std::cout << "7- Visualizar Reportes" << std::endl;
        std::cout << "8- Salir" << std::endl;
        std::cout << "---------------------------------------------------" << std::endl;
        std::cout << "Ingrese una opcion: ";
        std::cin >> opcion;

        
        std::cin.ignore();

        switch (opcion) {
        case 1: {
            std::string filename;
            std::cout << std::endl;
            std::cout << "------ CARGA DE AVIONES ------" << std::endl;
            std::cout << "Ingresa la ruta JSON de aviones: ";
            std::getline(std::cin, filename);

            cargarAvionesDesdeJSON(filename, arbolAviones, listaMantenimiento,avionesPorVuelo);
            break;
        }
        case 2: {
            std::string filename;
            std::cout << std::endl;
            std::cout << "------ CARGA DE PILOTOS ------" << std::endl;
            std::cout << "Ingresa la ruta JSON de pilotos: ";
            std::getline(std::cin, filename);

            cargarPilotos(filename, arbolPilotos, TablaHashPilotos,matrizDispersa,avionesPorVuelo);
            TablaHashPilotos.imprimirTabla();
            break;
        }


        case 3: {
            std::string filename;
            std::cout << std::endl;
            std::cout << "------ CARGA DE RUTAS ------" << std::endl;
            std::cout << "Ingresa la ruta JSON de rutas: ";
            std::getline(std::cin, filename);

            cargarRutas(filename, grafoRutas);
            break;
        }
        case 4: {
            std::string filename;
            std::cout << std::endl;
            std::cout << "------ CARGA DE MOVIMIENTOS ------" << std::endl;
            std::cout << "Ingresa la ruta JSON de movimientos: ";
            std::getline(std::cin, filename);

        
            CargarComandos(filename, arbolAviones, listaMantenimiento,arbolPilotos,TablaHashPilotos,matrizDispersa);

            break;
        }

        case 5: {
            std::cout << std::endl;
            std::cout << "------ CONSULTA DE HORAS DE VUELO (PILOTOS) ------" << std::endl;
            std::cout << std::endl;
            std::cout << "------- INGRESE EL ORDEN QUE DESEA VISUALIZAR -------" << std::endl;
            std::cout << "1- In-Orden" << std::endl;
            std::cout << "2- Pre-Orden" << std::endl;
            std::cout << "3- Post-Orden" << std::endl;
            std::cout << "Ingrese una opcion: ";
            int opcionOrden;
            std::cin >> opcionOrden;

            switch (opcionOrden) {
            case 1: {
                std::cout << "Recorrido In-Orden:" << std::endl;
                arbolPilotos.inOrden(); 
                break;
            }
            case 2: {
                std::cout << "Recorrido Pre-Orden:" << std::endl;
                arbolPilotos.preOrden(); 
                break;
            }
            case 3: {
                std::cout << "Recorrido Post-Orden:" << std::endl;
                arbolPilotos.postOrden(); 
                break;
            }
            default: {
                std::cout << "Opción no válida. Por favor, ingrese una opción válida." << std::endl;
                break;
            }
            }
            break;
        }


        case 6: {
            std::string origen, destino;
            std::cout << std::endl;
            std::cout << "------ RECOMENDAR RUTA ------" << std::endl;
            std::cout << "Ingresa el origen: ";
            std::getline(std::cin, origen);
            std::cout << "Ingresa el destino: ";
            std::getline(std::cin, destino);
            grafoRutas.encontrarRutaMasCorta(origen, destino);
            break;
        }
        case 7: {
            std::cout << std::endl;
            std::cout << "------ VISUALIZAR REPORTES ------" << std::endl;


            std::cout << "Tabla Hash de pilotos Generada con exito" << std::endl;
            TablaHashPilotos.generarDot("tabla_hash_pilotos.dot");

            std::cout << "Lista de aviones en mantenimiento:" << std::endl;
            listaMantenimiento.GenerarDot();

            std::cout << "Arbol de aviones Generado con exito" <<std::endl;
            arbolAviones.graficar("ArbolAviones");

            std::cout << "Recorrido de rutas Generado con exito" <<std::endl;
            grafoRutas.GrafoDirigidoParaRutas();

            std::cout << "Arbol de pilotos Generado con exito" <<std::endl;
            arbolPilotos.generarDot();

            std::cout << "Matriz de Vuelos generada con exito" << std::endl;
            matrizDispersa.ObtenerGraphviz();

            

            break;
        }
        case 8: {
            std::cout << std::endl;
            std::cout << "Saliendo del programa..." << std::endl;
            break;
        }
        default: {
            std::cout << std::endl;
            std::cout << "Opción no válida. Por favor, ingrese un número válido del menú." << std::endl;
            break;
        }
        }
    } while (opcion != 8);

    return 0;
}