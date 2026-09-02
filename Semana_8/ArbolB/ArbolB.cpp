#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

class StringBuilder {
private:
    std::string contenido;
public:
    StringBuilder() : contenido("") {}

    void agregar(const std::string& texto) {
        contenido += texto;
    }

    void agregar(int numero) {
        contenido += std::to_string(numero);
    }

    void agregar(void* puntero) {
        if (puntero == nullptr) {
            contenido += "0";
        }
        else {
            char buffer[32];
            std::snprintf(buffer, sizeof(buffer), "%p", puntero);
            std::string hexStr(buffer);
            std::string limpio = "Nodo";
            for (size_t i = 0; i < hexStr.size(); ++i) {
                char c = hexStr[i];
                if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    limpio += c;
                }
            }
            contenido += limpio;
        }
    }

    std::string str() const {
        return contenido;
    }
};

class Cancion {
private:
    std::string codigo;
    std::string nombre;
    std::string artista;

public:
    Cancion() : codigo(""), nombre(""), artista("") {}

    Cancion(const std::string& codigo, const std::string& nombre, const std::string& artista)
        : codigo(codigo), nombre(nombre), artista(artista) {}

    std::string getCodigo() const { return codigo; }
    std::string getNombre() const { return nombre; }
    std::string getArtista() const { return artista; }

    friend std::ostream& operator<<(std::ostream& os, const Cancion& cancion);
};

std::ostream& operator<<(std::ostream& os, const Cancion& cancion) {
    os << "Codigo: " << cancion.codigo << ", Nombre: " << cancion.nombre
        << ", Artista: " << cancion.artista;
    return os;
}

class NodoArbolB {
public:
    NodoArbolB(int m);
    ~NodoArbolB();

    bool esHoja() const;
    bool estaLleno() const;
    void insertarClave(const Cancion& nuevaCancion);
    NodoArbolB* dividirNodo(Cancion& mediana);
    int encontrarPosicionInsertar(const std::string& codigo);
    int buscarClave(const std::string& codigo);

private:
    int clavesUsadas;
    Cancion* clave;
    NodoArbolB** puntero;
    NodoArbolB* padre;
    NodoArbolB* siguiente;
    bool hoja;
    int m;

    friend class ArbolBCanciones;
};

NodoArbolB::NodoArbolB(int m)
    : clavesUsadas(0), clave(new Cancion[m - 1]), puntero(new NodoArbolB*[m]),
    padre(nullptr), siguiente(nullptr), hoja(true), m(m) {
    for (int i = 0; i < m; ++i) {
        puntero[i] = nullptr;
    }
}

NodoArbolB::~NodoArbolB() {
    delete[] clave;
    delete[] puntero;
}

bool NodoArbolB::esHoja() const {
    return hoja;
}

bool NodoArbolB::estaLleno() const {
    return clavesUsadas == (m - 1);
}

int NodoArbolB::encontrarPosicionInsertar(const std::string& codigo) {
    int i = 0;
    while (i < clavesUsadas && clave[i].getCodigo() < codigo) {
        ++i;
    }
    return i;
}

void NodoArbolB::insertarClave(const Cancion& nuevaCancion) {
    int i = encontrarPosicionInsertar(nuevaCancion.getCodigo());
    for (int j = clavesUsadas; j > i; --j) {
        clave[j] = clave[j - 1];
    }
    clave[i] = nuevaCancion;
    ++clavesUsadas;
}

NodoArbolB* NodoArbolB::dividirNodo(Cancion& mediana) {
    int mid = clavesUsadas / 2;
    NodoArbolB* nuevoNodo = new NodoArbolB(m);
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
    nuevoNodo->padre = padre;
    nuevoNodo->siguiente = this->siguiente;
    this->siguiente = nuevoNodo;

    return nuevoNodo;
}

int NodoArbolB::buscarClave(const std::string& codigo) {
    int idx = 0;
    while (idx < clavesUsadas && clave[idx].getCodigo() < codigo) {
        ++idx;
    }
    return idx;
}

class ArbolBCanciones {
public:
    ArbolBCanciones(int m);
    ~ArbolBCanciones();

    bool insertar(const Cancion& nuevaCancion);
    Cancion* buscar(const std::string& codigo);
    bool eliminar(const std::string& codigo);
    void graficar();
    bool estaVacio() const;

private:
    NodoArbolB* raiz;
    int m;

    void insertarNoLleno(NodoArbolB* nodo, const Cancion& nuevaCancion);
    Cancion* buscarEnNodo(NodoArbolB* nodo, const std::string& codigo);
    int minClaves() const;
    void eliminarDeNodo(NodoArbolB* nodo, const std::string& codigo);
    void eliminarDeHoja(NodoArbolB* nodo, int idx);
    void eliminarDeInterno(NodoArbolB* nodo, int idx);
    Cancion obtenerMaximo(NodoArbolB* nodo);
    Cancion obtenerMinimo(NodoArbolB* nodo);
    void llenarHijo(NodoArbolB* nodo, int idx);
    void fusionarHijos(NodoArbolB* nodo, int idx);
    void destruirNodo(NodoArbolB* nodo);
    std::string obtenerDot();
    void graficarNodo(StringBuilder& ss, NodoArbolB* nodo, int& nullCount);
};

ArbolBCanciones::ArbolBCanciones(int m) : raiz(new NodoArbolB(m)), m(m) {}

ArbolBCanciones::~ArbolBCanciones() {
    destruirNodo(raiz);
}

bool ArbolBCanciones::estaVacio() const {
    return raiz->clavesUsadas == 0;
}

bool ArbolBCanciones::insertar(const Cancion& nuevaCancion) {
    if (buscar(nuevaCancion.getCodigo()) != nullptr) {
        std::cout << "Ya existe una cancion con el codigo '" << nuevaCancion.getCodigo() << "'." << std::endl;
        return false;
    }

    if (raiz->estaLleno()) {
        NodoArbolB* nuevaRaiz = new NodoArbolB(m);
        nuevaRaiz->hoja = false;
        nuevaRaiz->puntero[0] = raiz;
        raiz->padre = nuevaRaiz;
        Cancion mediana;
        NodoArbolB* nuevoNodo = raiz->dividirNodo(mediana);
        nuevoNodo->padre = nuevaRaiz;
        nuevaRaiz->clave[0] = mediana;
        nuevaRaiz->puntero[1] = nuevoNodo;
        nuevaRaiz->clavesUsadas = 1;
        raiz = nuevaRaiz;
    }
    insertarNoLleno(raiz, nuevaCancion);
    return true;
}

void ArbolBCanciones::insertarNoLleno(NodoArbolB* nodo, const Cancion& nuevaCancion) {
    if (nodo == nullptr) {
        std::cerr << "Error: nodo es nullptr en insertarNoLleno." << std::endl;
        return;
    }

    if (nodo->esHoja()) {
        nodo->insertarClave(nuevaCancion);
    }
    else {
        int i = nodo->encontrarPosicionInsertar(nuevaCancion.getCodigo());
        if (nodo->puntero[i] == nullptr) {
            std::cerr << "Error: nodo->puntero[" << i << "] es nullptr en insertarNoLleno." << std::endl;
            return;
        }

        if (nodo->puntero[i]->estaLleno()) {
            Cancion mediana;
            NodoArbolB* nuevoNodo = nodo->puntero[i]->dividirNodo(mediana);
            nuevoNodo->padre = nodo;
            nodo->insertarClave(mediana);

            for (int j = nodo->clavesUsadas; j > i + 1; --j) {
                nodo->puntero[j] = nodo->puntero[j - 1];
            }

            nodo->puntero[i + 1] = nuevoNodo;

            if (nuevaCancion.getCodigo() > mediana.getCodigo()) {
                ++i;
            }
        }

        insertarNoLleno(nodo->puntero[i], nuevaCancion);
    }
}

Cancion* ArbolBCanciones::buscar(const std::string& codigo) {
    return buscarEnNodo(raiz, codigo);
}

Cancion* ArbolBCanciones::buscarEnNodo(NodoArbolB* nodo, const std::string& codigo) {
    if (!nodo) return nullptr;

    int idx = nodo->buscarClave(codigo);
    if (idx < nodo->clavesUsadas && nodo->clave[idx].getCodigo() == codigo) {
        return &nodo->clave[idx];
    }

    if (nodo->esHoja()) {
        return nullptr;
    }
    else {
        return buscarEnNodo(nodo->puntero[idx], codigo);
    }
}

int ArbolBCanciones::minClaves() const {
    return (m + 1) / 2 - 1;
}

bool ArbolBCanciones::eliminar(const std::string& codigo) {
    if (buscar(codigo) == nullptr) {
        std::cout << "No existe una cancion con el codigo '" << codigo << "'." << std::endl;
        return false;
    }

    eliminarDeNodo(raiz, codigo);

    while (!raiz->esHoja() && raiz->clavesUsadas == 0) {
        NodoArbolB* viejaRaiz = raiz;
        raiz = raiz->puntero[0];
        raiz->padre = nullptr;
        delete viejaRaiz;
    }

    std::cout << "Se elimino la cancion con el codigo '" << codigo << "'." << std::endl;
    return true;
}

void ArbolBCanciones::eliminarDeNodo(NodoArbolB* nodo, const std::string& codigo) {
    int idx = nodo->buscarClave(codigo);
    bool encontrada = idx < nodo->clavesUsadas && nodo->clave[idx].getCodigo() == codigo;

    if (nodo->esHoja()) {
        if (encontrada) {
            eliminarDeHoja(nodo, idx);
        }
        return;
    }

    if (encontrada) {
        eliminarDeInterno(nodo, idx);
        return;
    }

    bool ultima = (idx == nodo->clavesUsadas);
    if (nodo->puntero[idx]->clavesUsadas <= minClaves()) {
        llenarHijo(nodo, idx);
        if (ultima && idx > nodo->clavesUsadas) {
            idx = nodo->clavesUsadas;
        }
    }
    eliminarDeNodo(nodo->puntero[idx], codigo);
}

void ArbolBCanciones::eliminarDeHoja(NodoArbolB* nodo, int idx) {
    for (int j = idx; j < nodo->clavesUsadas - 1; ++j) {
        nodo->clave[j] = nodo->clave[j + 1];
    }
    nodo->clavesUsadas--;
}

void ArbolBCanciones::eliminarDeInterno(NodoArbolB* nodo, int idx) {
    Cancion objetivo = nodo->clave[idx];

    if (nodo->puntero[idx]->clavesUsadas > minClaves()) {
        Cancion predecesor = obtenerMaximo(nodo->puntero[idx]);
        nodo->clave[idx] = predecesor;
        eliminarDeNodo(nodo->puntero[idx], predecesor.getCodigo());
    }
    else if (nodo->puntero[idx + 1]->clavesUsadas > minClaves()) {
        Cancion sucesor = obtenerMinimo(nodo->puntero[idx + 1]);
        nodo->clave[idx] = sucesor;
        eliminarDeNodo(nodo->puntero[idx + 1], sucesor.getCodigo());
    }
    else {
        fusionarHijos(nodo, idx);
        eliminarDeNodo(nodo->puntero[idx], objetivo.getCodigo());
    }
}

Cancion ArbolBCanciones::obtenerMaximo(NodoArbolB* nodo) {
    while (!nodo->esHoja()) {
        nodo = nodo->puntero[nodo->clavesUsadas];
    }
    return nodo->clave[nodo->clavesUsadas - 1];
}

Cancion ArbolBCanciones::obtenerMinimo(NodoArbolB* nodo) {
    while (!nodo->esHoja()) {
        nodo = nodo->puntero[0];
    }
    return nodo->clave[0];
}

void ArbolBCanciones::llenarHijo(NodoArbolB* nodo, int idx) {
    NodoArbolB* hijo = nodo->puntero[idx];

    if (idx != 0 && nodo->puntero[idx - 1]->clavesUsadas > minClaves()) {
        NodoArbolB* hermano = nodo->puntero[idx - 1];

        for (int j = hijo->clavesUsadas; j > 0; --j) {
            hijo->clave[j] = hijo->clave[j - 1];
        }
        if (!hijo->esHoja()) {
            for (int j = hijo->clavesUsadas + 1; j > 0; --j) {
                hijo->puntero[j] = hijo->puntero[j - 1];
            }
            hijo->puntero[0] = hermano->puntero[hermano->clavesUsadas];
            hijo->puntero[0]->padre = hijo;
        }
        hijo->clave[0] = nodo->clave[idx - 1];
        hijo->clavesUsadas++;
        nodo->clave[idx - 1] = hermano->clave[hermano->clavesUsadas - 1];
        hermano->clavesUsadas--;
    }
    else if (idx != nodo->clavesUsadas && nodo->puntero[idx + 1]->clavesUsadas > minClaves()) {
        NodoArbolB* hermano = nodo->puntero[idx + 1];

        hijo->clave[hijo->clavesUsadas] = nodo->clave[idx];
        if (!hijo->esHoja()) {
            hijo->puntero[hijo->clavesUsadas + 1] = hermano->puntero[0];
            hijo->puntero[hijo->clavesUsadas + 1]->padre = hijo;
        }
        hijo->clavesUsadas++;
        nodo->clave[idx] = hermano->clave[0];

        for (int j = 0; j < hermano->clavesUsadas - 1; ++j) {
            hermano->clave[j] = hermano->clave[j + 1];
        }
        if (!hermano->esHoja()) {
            for (int j = 0; j < hermano->clavesUsadas; ++j) {
                hermano->puntero[j] = hermano->puntero[j + 1];
            }
        }
        hermano->clavesUsadas--;
    }
    else {
        if (idx != nodo->clavesUsadas) {
            fusionarHijos(nodo, idx);
        }
        else {
            fusionarHijos(nodo, idx - 1);
        }
    }
}

void ArbolBCanciones::fusionarHijos(NodoArbolB* nodo, int idx) {
    NodoArbolB* hijo = nodo->puntero[idx];
    NodoArbolB* hermano = nodo->puntero[idx + 1];

    hijo->clave[hijo->clavesUsadas] = nodo->clave[idx];
    for (int j = 0; j < hermano->clavesUsadas; ++j) {
        hijo->clave[hijo->clavesUsadas + 1 + j] = hermano->clave[j];
    }
    if (!hijo->esHoja()) {
        for (int j = 0; j <= hermano->clavesUsadas; ++j) {
            hijo->puntero[hijo->clavesUsadas + 1 + j] = hermano->puntero[j];
            hijo->puntero[hijo->clavesUsadas + 1 + j]->padre = hijo;
        }
    }
    hijo->clavesUsadas += hermano->clavesUsadas + 1;
    hijo->siguiente = hermano->siguiente;

    for (int j = idx; j < nodo->clavesUsadas - 1; ++j) {
        nodo->clave[j] = nodo->clave[j + 1];
    }
    if (!nodo->esHoja()) {
        for (int j = idx + 1; j < nodo->clavesUsadas; ++j) {
            nodo->puntero[j] = nodo->puntero[j + 1];
        }
    }
    nodo->clavesUsadas--;

    delete hermano;
}

void ArbolBCanciones::destruirNodo(NodoArbolB* nodo) {
    if (nodo) {
        if (!nodo->esHoja()) {
            for (int i = 0; i <= nodo->clavesUsadas; ++i) {
                destruirNodo(nodo->puntero[i]);
            }
        }
        delete nodo;
    }
}

void ArbolBCanciones::graficarNodo(StringBuilder& ss, NodoArbolB* nodo, int& nullCount) {
    if (!nodo) return;

    ss.agregar("Nodo");
    ss.agregar(nodo);
    ss.agregar(" [shape=record, label=\"");
    for (int i = 0; i < nodo->clavesUsadas; ++i) {
        ss.agregar("<f");
        ss.agregar(i);
        ss.agregar("> ");
        ss.agregar(nodo->clave[i].getCodigo());
        ss.agregar(" - ");
        ss.agregar(nodo->clave[i].getNombre());
        if (i < nodo->clavesUsadas - 1) {
            ss.agregar(" | ");
        }
    }
    ss.agregar("\"];\n");

    if (!nodo->esHoja()) {
        for (int i = 0; i <= nodo->clavesUsadas; ++i) {
            if (nodo->puntero[i]) {
                ss.agregar("Nodo");
                ss.agregar(nodo);
                ss.agregar(" -> Nodo");
                ss.agregar(nodo->puntero[i]);
                ss.agregar(";\n");
                graficarNodo(ss, nodo->puntero[i], nullCount);
            }
            else {
                ss.agregar("Nodo");
                ss.agregar(nodo);
                ss.agregar(" -> null");
                ss.agregar(nullCount);
                ss.agregar(";\n");
                ss.agregar("null");
                ss.agregar(nullCount);
                ss.agregar(" [shape=point];\n");
                nullCount++;
            }
        }
    }
}

std::string ArbolBCanciones::obtenerDot() {
    StringBuilder ss;
    ss.agregar("digraph ArbolBCanciones {\n");
    ss.agregar("bgcolor=lightblue;\n");
    ss.agregar("node [shape=box, style=filled, fillcolor=beige, color=black, penwidth=2];\n");
    ss.agregar("edge [splines=polyline, arrowhead=curve];\n");
    if (raiz) {
        int nullCount = 0;
        graficarNodo(ss, raiz, nullCount);
    }
    ss.agregar("}\n");
    return ss.str();
}

void ArbolBCanciones::graficar() {
    if (estaVacio()) {
        std::cout << "No hay canciones en el arbol." << std::endl;
        return;
    }

    std::ofstream archivo("arbol_b.dot");
    archivo << obtenerDot();
    archivo.close();

    system("dot -Tpng arbol_b.dot -o arbol_b_canciones.png");
    system("start arbol_b_canciones.png");
}

void saltarEspacios(const std::string& texto, size_t& pos) {
    while (pos < texto.size() && isspace(static_cast<unsigned char>(texto[pos]))) {
        ++pos;
    }
}

bool leerStringJSON(const std::string& texto, size_t& pos, std::string& resultado) {
    if (pos >= texto.size() || texto[pos] != '"') {
        return false;
    }
    ++pos;
    resultado.clear();
    while (pos < texto.size()) {
        char c = texto[pos];
        if (c == '\\') {
            ++pos;
            if (pos >= texto.size()) {
                return false;
            }
            switch (texto[pos]) {
            case '"': resultado += '"'; break;
            case '\\': resultado += '\\'; break;
            case '/': resultado += '/'; break;
            case 'n': resultado += '\n'; break;
            case 't': resultado += '\t'; break;
            case 'r': resultado += '\r'; break;
            default: resultado += texto[pos]; break;
            }
            ++pos;
        }
        else if (c == '"') {
            ++pos;
            return true;
        }
        else {
            resultado += c;
            ++pos;
        }
    }
    return false;
}

std::string quitarBOM(const std::string& contenido) {
    if (contenido.size() >= 3 &&
        static_cast<unsigned char>(contenido[0]) == 0xEF &&
        static_cast<unsigned char>(contenido[1]) == 0xBB &&
        static_cast<unsigned char>(contenido[2]) == 0xBF) {
        return contenido.substr(3);
    }
    return contenido;
}

bool cargarJSON(const std::string& ruta, ArbolBCanciones& arbol) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo '" << ruta << "'." << std::endl;
        return false;
    }

    std::string texto;
    {
        char c;
        while (archivo.get(c)) {
            texto += c;
        }
    }
    archivo.close();
    texto = quitarBOM(texto);
    size_t pos = 0;
    int insertados = 0;

    saltarEspacios(texto, pos);
    if (pos >= texto.size() || texto[pos] != '[') {
        std::cout << "El archivo no contiene un arreglo JSON valido." << std::endl;
        return false;
    }
    ++pos;

    while (true) {
        saltarEspacios(texto, pos);
        if (pos >= texto.size()) {
            std::cout << "Formato JSON incompleto." << std::endl;
            return false;
        }
        if (texto[pos] == ']') {
            break;
        }

        if (texto[pos] != '{') {
            std::cout << "Se esperaba un objeto en la posicion " << pos << "." << std::endl;
            return false;
        }
        ++pos;

        std::string codigo, nombre, artista;

        while (true) {
            saltarEspacios(texto, pos);
            std::string clave;
            if (!leerStringJSON(texto, pos, clave)) {
                std::cout << "Clave invalida en la posicion " << pos << "." << std::endl;
                return false;
            }

            saltarEspacios(texto, pos);
            if (pos >= texto.size() || texto[pos] != ':') {
                std::cout << "Se esperaba ':' en la posicion " << pos << "." << std::endl;
                return false;
            }
            ++pos;
            saltarEspacios(texto, pos);

            std::string valor;
            if (!leerStringJSON(texto, pos, valor)) {
                std::cout << "Valor invalido para '" << clave << "' en la posicion " << pos << "." << std::endl;
                return false;
            }

            if (clave == "codigo") codigo = valor;
            else if (clave == "nombre") nombre = valor;
            else if (clave == "artista") artista = valor;

            saltarEspacios(texto, pos);
            if (pos < texto.size() && texto[pos] == ',') {
                ++pos;
                continue;
            }
            if (pos < texto.size() && texto[pos] == '}') {
                ++pos;
                break;
            }
            std::cout << "Se esperaba ',' o '}' en la posicion " << pos << "." << std::endl;
            return false;
        }

        if (!codigo.empty()) {
            if (arbol.insertar(Cancion(codigo, nombre, artista))) {
                insertados++;
            }
        }

        saltarEspacios(texto, pos);
        if (pos < texto.size() && texto[pos] == ',') {
            ++pos;
            continue;
        }
    }

    std::cout << "Se cargaron " << insertados << " canciones desde '" << ruta << "'." << std::endl;
    return true;
}

std::string quitarComillas(const std::string& campo) {
    std::string resultado = campo;
    if (resultado.size() >= 2 && resultado.front() == '"' && resultado.back() == '"') {
        resultado = resultado.substr(1, resultado.size() - 2);
    }
    return resultado;
}

bool cargarCSV(const std::string& ruta, ArbolBCanciones& arbol) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo '" << ruta << "'." << std::endl;
        return false;
    }

    std::string linea;
    bool encabezado = true;
    int insertados = 0;

    while (getline(archivo, linea)) {
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }
        if (linea.empty()) {
            continue;
        }

        if (encabezado) {
            encabezado = false;
            continue;
        }

        size_t inicio = 0;
        std::string campos[50];
        int numCampos = 0;
        for (size_t i = 0; i <= linea.size(); ++i) {
            if (i == linea.size() || linea[i] == ',') {
                if (numCampos < 50) {
                    campos[numCampos++] = quitarComillas(linea.substr(inicio, i - inicio));
                }
                inicio = i + 1;
            }
        }

        if (numCampos >= 3) {
            std::string codigo = campos[0];
            std::string nombre = campos[1];
            std::string artista = campos[2];
            for (int i = 3; i < numCampos; ++i) {
                artista += "," + campos[i];
            }

            if (arbol.insertar(Cancion(codigo, nombre, artista))) {
                insertados++;
            }
        }
    }

    archivo.close();
    std::cout << "Se cargaron " << insertados << " canciones desde '" << ruta << "'." << std::endl;
    return true;
}

void leerLinea(std::string& linea) {
    std::getline(std::cin, linea);
    if (!linea.empty() && linea.back() == '\r') {
        linea.pop_back();
    }
}

int main() {
    ArbolBCanciones arbol(4);
    int opcion;

    do {
        std::cout << std::endl;
        std::cout << "========== M E N U ===========" << std::endl;
        std::cout << "1. Insertar cancion" << std::endl;
        std::cout << "2. Cargar JSON" << std::endl;
        std::cout << "3. Cargar CSV" << std::endl;
        std::cout << "4. Eliminar cancion" << std::endl;
        std::cout << "5. Graficar Arbol" << std::endl;
        std::cout << "6. Salir" << std::endl;
        std::cout << "Ingrese una opcion: ";
        std::cin >> opcion;
        std::cin.ignore(999999, '\n');

        switch (opcion) {
        case 1: {
            std::string codigo, nombre, artista;

            std::cout << "Ingrese el codigo: ";
            leerLinea(codigo);
            std::cout << "Ingrese el nombre: ";
            leerLinea(nombre);
            std::cout << "Ingrese el artista: ";
            leerLinea(artista);

            arbol.insertar(Cancion(codigo, nombre, artista));
            break;
        }
        case 2: {
            std::string ruta;
            std::cout << "Ingrese la ruta del archivo JSON: ";
            leerLinea(ruta);

            cargarJSON(ruta, arbol);
            break;
        }
        case 3: {
            std::string ruta;
            std::cout << "Ingrese la ruta del archivo CSV: ";
            leerLinea(ruta);

            cargarCSV(ruta, arbol);
            break;
        }
        case 4: {
            std::string codigo;
            std::cout << "Ingrese el codigo de la cancion a eliminar: ";
            leerLinea(codigo);

            arbol.eliminar(codigo);
            break;
        }
        case 5:
            arbol.graficar();
            break;
        case 6:
            std::cout << "Saliendo del programa..." << std::endl;
            break;
        default:
            std::cout << "Opcion no valida." << std::endl;
            break;
        }
    } while (opcion != 6);

    return 0;
}
