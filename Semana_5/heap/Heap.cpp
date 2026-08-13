#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

// Piloto guarda los datos de un piloto: nombre, nacionalidad y horas de vuelo
class Piloto {
private:
    std::string nombre;
    std::string nacionalidad;
    int horas_de_vuelo;

public:
    Piloto() : nombre(""), nacionalidad(""), horas_de_vuelo(0) {}

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

// El heap se ordena por horas de vuelo: la raiz es el piloto con mas horas
// Los hijos de cada nodo estan en las posiciones que le siguen dentro del arreglo
// y su padre esta en una posicion anterior
class HeapMaxPilotos {
private:
    vector<Piloto> datos;

    // sube un nodo hasta su posicion correcta dentro del heap
    void flotar(int indice) {
        while (indice > 0 && datos[(indice - 1) / 2].getHorasDeVuelo() < datos[indice].getHorasDeVuelo()) {
            swap(datos[(indice - 1) / 2], datos[indice]);
            indice = (indice - 1) / 2;
        }
    }

    // baja un nodo hasta su posicion correcta dentro del heap
    void hundir(int indice) {
        int tam = datos.size();
        while (true) {
            int mayor = indice;
            int izq = 2 * indice + 1;
            int der = 2 * indice + 2;

            if (izq < tam && datos[izq].getHorasDeVuelo() > datos[mayor].getHorasDeVuelo()) {
                mayor = izq;
            }
            if (der < tam && datos[der].getHorasDeVuelo() > datos[mayor].getHorasDeVuelo()) {
                mayor = der;
            }
            if (mayor == indice) {
                break;
            }
            swap(datos[indice], datos[mayor]);
            indice = mayor;
        }
    }

    // quita las comillas que envuelven un campo al leer el CSV
    std::string quitarComillas(const std::string& campo) {
        std::string resultado = campo;
        if (resultado.size() >= 2 && resultado.front() == '"' && resultado.back() == '"') {
            resultado = resultado.substr(1, resultado.size() - 2);
        }
        return resultado;
    }

public:
    HeapMaxPilotos() {}

    bool estaVacio() {
        return datos.empty();
    }

    int tamano() {
        return datos.size();
    }

    // inserta un piloto al final y lo sube hasta su posicion correcta
    void insertar(const Piloto& piloto) {
        datos.push_back(piloto);
        flotar(datos.size() - 1);
    }

    // construye el heap desde el arreglo actual aplicando el algoritmo estandar
    // de heapify, hundiendo cada nodo hasta su posicion correcta empezando desde el final
    void heapify() {
        int tam = datos.size();
        for (int i = tam / 2 - 1; i >= 0; --i) {
            hundir(i);
        }
    }

    // elimina y devuelve el piloto con mas horas de vuelo, el que esta en la raiz
    Piloto eliminarMax() {
        if (estaVacio()) {
            cout << "El heap esta vacio." << endl;
            return Piloto();
        }

        Piloto maximo = datos[0];
        datos[0] = datos.back();
        datos.pop_back();

        if (!estaVacio()) {
            hundir(0);
        }

        return maximo;
    }

    // imprime el arreglo del heap y la propiedad de cada nodo
    void mostrar() {
        if (estaVacio()) {
            cout << "El heap esta vacio." << endl;
            return;
        }

        cout << "Heap (arreglo) por horas de vuelo:" << endl;
        for (size_t i = 0; i < datos.size(); ++i) {
            cout << "Nodo[" << i << "] " << datos[i] << endl;
        }
        cout << endl;

        cout << "Estructura de arbol:" << endl;
        for (size_t i = 0; i < datos.size(); ++i) {
            cout << "Nodo[" << i << "] = " << datos[i].getNombre() << " (" << datos[i].getHorasDeVuelo() << "h)";
            if (2 * i + 1 < datos.size()) {
                cout << " -> Hijo izquierdo: " << datos[2 * i + 1].getNombre() << " (" << datos[2 * i + 1].getHorasDeVuelo() << "h)";
            }
            if (2 * i + 2 < datos.size()) {
                cout << ", Hijo derecho: " << datos[2 * i + 2].getNombre() << " (" << datos[2 * i + 2].getHorasDeVuelo() << "h)";
            }
            cout << endl;
        }
    }

    // ordena los pilotos de mayor a menor horas de vuelo copiando el heap sin modificar el original
    vector<Piloto> heapSort() {
        HeapMaxPilotos copia = *this;
        vector<Piloto> ordenado;
        while (!copia.estaVacio()) {
            ordenado.push_back(copia.eliminarMax());
        }
        return ordenado;
    }

    // carga los pilotos desde un archivo CSV con formato nombre,nacionalidad,horas_de_vuelo
    void cargarCSV(const string& ruta) {
        ifstream archivo(ruta);
        if (!archivo.is_open()) {
            cout << "No se pudo abrir el archivo '" << ruta << "'." << endl;
            return;
        }

        string linea;
        bool encabezado = true;
        int insertados = 0;
        vector<Piloto> cargados;

        while (getline(archivo, linea)) {
            if (linea.empty()) {
                continue;
            }

            if (encabezado) {
                encabezado = false;
                continue;
            }

            size_t inicio = 0;
            vector<string> campos;
            for (size_t i = 0; i <= linea.size(); ++i) {
                if (i == linea.size() || linea[i] == ',') {
                    campos.push_back(quitarComillas(linea.substr(inicio, i - inicio)));
                    inicio = i + 1;
                }
            }

            if (campos.size() >= 3) {
                string nombre = campos[0];
                string nacionalidad = campos[1];
                int horas_de_vuelo = atoi(campos[2].c_str());
                cargados.push_back(Piloto(nombre, nacionalidad, horas_de_vuelo));
                insertados++;
            }
        }

        archivo.close();

        // construye el heap con el algoritmo estandar de heapify sobre el arreglo cargado
        datos = cargados;
        heapify();

        cout << "Se cargaron " << insertados << " pilotos desde '" << ruta << "'." << endl;
    }

    // genera el archivo .dot del heap y lo compila a PNG con Graphviz
    void generarDot() {
        ofstream archivo("Heap.dot");
        archivo << "digraph HeapMaxPilotos {\n";
        archivo << "rankdir=TB;\n";
        archivo << "bgcolor=\"#f7f9fc\";\n";
        archivo << "node [shape=box, style=filled, fillcolor=\"#16a085\", fontcolor=white, fontname=\"Arial\", fontsize=12, color=\"#117a65\", penwidth=1.5];\n";
        archivo << "edge [color=\"#5d6d7e\", arrowsize=0.8, penwidth=1.4];\n";
        archivo << "label=\"Max Heap de Pilotos (por horas de vuelo)\";\n";
        archivo << "labelloc=t;\n";

        for (size_t i = 0; i < datos.size(); ++i) {
            archivo << "N" << i << " [label=\"" << datos[i].getNombre() << "\\n" << datos[i].getHorasDeVuelo() << " horas\"];\n";
            if (2 * i + 1 < datos.size()) {
                archivo << "N" << i << " -> N" << 2 * i + 1 << ";\n";
            }
            if (2 * i + 2 < datos.size()) {
                archivo << "N" << i << " -> N" << 2 * i + 2 << ";\n";
            }
        }

        archivo << "}\n";
        archivo.close();

        system("dot -Tpng Heap.dot -o heap_de_pilotos.png");
        system("start heap_de_pilotos.png");
    }
};

int main() {
    HeapMaxPilotos heap;
    int opcion;

    do {
        cout << endl;
        cout << "========== M E N U ===========" << endl;
        cout << "1. Insertar piloto" << endl;
        cout << "2. Eliminar maximo" << endl;
        cout << "3. Ver pilotos" << endl;
        cout << "4. Heap sort" << endl;
        cout << "5. Cargar CSV" << endl;
        cout << "6. Generar reporte de pilotos" << endl;
        cout << "7. Salir" << endl;
        cout << "Ingrese una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        case 1: {
            string nombre, nacionalidad;
            int horas_de_vuelo;

            cout << "Ingrese el nombre: ";
            getline(cin, nombre);
            cout << "Ingrese la nacionalidad: ";
            getline(cin, nacionalidad);
            cout << "Ingrese las horas de vuelo: ";
            cin >> horas_de_vuelo;
            cin.ignore();

            heap.insertar(Piloto(nombre, nacionalidad, horas_de_vuelo));
            cout << "Piloto insertado correctamente." << endl;
            break;
        }
        case 2: {
            Piloto maximo = heap.eliminarMax();
            if (!maximo.getNombre().empty()) {
                cout << "Piloto con mas horas eliminado: " << maximo << endl;
            }
            break;
        }
        case 3:
            heap.mostrar();
            break;
        case 4: {
            vector<Piloto> ordenado = heap.heapSort();
            cout << "===== PILOTOS (HEAP SORT, MAYOR A MENOR HORAS) =====" << endl;
            if (ordenado.empty()) {
                cout << "El heap esta vacio." << endl;
            }
            for (size_t i = 0; i < ordenado.size(); ++i) {
                cout << ordenado[i] << endl;
            }
            break;
        }
        case 5: {
            string ruta;
            cout << "Ingrese la ruta del archivo CSV: ";
            getline(cin, ruta);

            heap.cargarCSV(ruta);
            break;
        }
        case 6:
            heap.generarDot();
            cout << "Reporte de pilotos generado correctamente." << endl;
            break;
        case 7:
            cout << "Saliendo del programa..." << endl;
            break;
        default:
            cout << "Opcion no valida." << endl;
            break;
        }
    } while (opcion != 7);

    return 0;
}
