#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>

using namespace std;

// Usamos UN SOLO metodo de hash: la polinomica mod m (regla de Horner, base 31).
class Nodo {
public:
    string artista;   // nombre del artista (la clave de la tabla)
    int edad;         // edad del artista en anios (el dato que leimos)
    Nodo* siguiente;

    // Todo nodo nuevo arranca apuntando a nullptr porque aun no sabemos si
    // habra mas canciones en el mismo cubo.
    Nodo(string art, int ed) : artista(art), edad(ed), siguiente(nullptr) {}
};

class TablaHash {
public:
    TablaHash(int n);
    ~TablaHash();

    // Devuelve el nombre del metodo hash que usamos (para el reporte).
    string nombreMetodoHash();

    // Convierte el nombre del artista en un indice entre 0 y tamano-1.
    int hash(string clave);

    // Guarda una cancion en la tabla segun el nombre del artista.
    void insertar(string artista, int edad);

    // Busca canciones por nombre de artista (imprime todas las que coincidan).
    void buscar(string artista);

    // Elimina una cancion de la tabla por nombre de artista.
    void eliminar(string artista);

    // Lee un archivo CSV formato artista,edad y mete todas las canciones.
    void cargarCSV(string ruta);

    // Muestra cada cubo con su lista de canciones en la consola.
    void verTabla();

    // Genera la imagen de la tabla con Graphviz.
    bool graficar();

private:
    Nodo** cubos;      // arreglo de punteros (cada cubo es una lista)
    int tamano;        // cuantos cubos (buckets) tiene la tabla

    // Devuelve true si el bucket tiene una cancion en el nivel dado.
    bool existeEnNivel(int bucket, int nivel);
};

// El constructor reserva el arreglo de cubos y deja cada posicion en nullptr.
TablaHash::TablaHash(int n) : tamano(n) {
    cubos = new Nodo*[tamano];
    for (int i = 0; i < tamano; i++) {
        cubos[i] = nullptr;
    }
}

// El destructor libera la memoria de cada lista dentro de los cubos.
TablaHash::~TablaHash() {
    for (int i = 0; i < tamano; i++) {
        Nodo* actual = cubos[i];
        while (actual != nullptr) {
            Nodo* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
    }
    delete[] cubos;
}

// Nombre del metodo de hash que usamos (siempre la polinomica).
string TablaHash::nombreMetodoHash() {
    return "Polinomica mod m (Horner, base 31)";
}

// Hash polinomico "a manita": recorre el nombre letra por letra con la
// regla de Horner en base 31, haciendo modulo en cada paso.
int TablaHash::hash(string clave) {
    unsigned long long h = 0;
    for (unsigned char c : clave) h = (h * 31 + c) % tamano;
    return (int)(h % tamano);
}

// Calcula el indice con el nombre del artista y agrega el nodo al inicio
// del cubo, porque insertar al inicio es de O(1) y no hay que recorrer nada.
void TablaHash::insertar(string artista, int edad) {
    int indice = hash(artista);
    Nodo* nuevo = new Nodo(artista, edad);
    nuevo->siguiente = cubos[indice];
    cubos[indice] = nuevo;
}

// Busca en el cubo del artista y muestra todas las canciones que coincidan.
void TablaHash::buscar(string artista) {
    int indice = hash(artista);
    Nodo* actual = cubos[indice];
    int encontradas = 0;

    cout << "Buscando canciones de \"" << artista << "\" (indice " << indice << ")..." << endl;
    while (actual != nullptr) {
        if (actual->artista == artista) {
            cout << "  - " << actual->artista << " (" << actual->edad << " anios)" << endl;
            encontradas++;
        }
        actual = actual->siguiente;
    }

    if (encontradas == 0) {
        cout << "  No hay canciones de ese artista." << endl;
    } else {
        cout << "  Total: " << encontradas << " cancion(es)." << endl;
    }
}

// Recorre la lista del cubo y elimina la PRIMERA cancion que coincida,
// enlazando el anterior con el siguiente para saltarse el nodo.
void TablaHash::eliminar(string artista) {
    int indice = hash(artista);
    Nodo* actual = cubos[indice];
    Nodo* anterior = nullptr;

    while (actual != nullptr && actual->artista != artista) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == nullptr) {
        cout << "No existe una cancion de \"" << artista << "\"." << endl;
        return;
    }

    if (anterior == nullptr) {
        cubos[indice] = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    cout << "Cancion de \"" << actual->artista << "\" (" << actual->edad
         << " anios) eliminada." << endl;
    delete actual;
}

// Recorre la lista del bucket hasta el nivel dado y dice si ahi hay cancion.
bool TablaHash::existeEnNivel(int bucket, int nivel) {
    Nodo* actual = cubos[bucket];
    for (int i = 0; i < nivel; i++) {
        if (actual == nullptr) return false;
        actual = actual->siguiente;
    }
    return actual != nullptr;
}

// Lee el CSV linea por linea (artista,edad), ignora el encabezado y las
// lineas invalidas, y manda cada cancion a insertar.
void TablaHash::cargarCSV(string ruta) {
    ifstream archivo(ruta.c_str());
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo '" << ruta << "'. Verifica la ruta." << endl;
        return;
    }

    string linea;
    int cargadas = 0;
    int ignoradas = 0;

    while (getline(archivo, linea)) {
        if (linea.empty()) continue;

        string nombre;
        string resto = linea;

        // Si el nombre empieza con comilla doble, el nombre es lo que esta
        // entre las comillas (puede contener comas).
        if (linea[0] == '"') {
            size_t cierre = linea.find('"', 1);
            if (cierre == string::npos) {
                ignoradas++;
                continue;
            }
            nombre = linea.substr(1, cierre - 1);
            resto = linea.substr(cierre + 1);
            if (!resto.empty() && resto[0] == ',') resto = resto.substr(1);
        } else {
            // Sin comillas: el nombre es todo lo que va antes de la primera coma.
            size_t coma = linea.find(',');
            if (coma == string::npos) {
                ignoradas++;
                continue;
            }
            nombre = linea.substr(0, coma);
            resto = linea.substr(coma + 1);
        }

        // La edad es lo que quedo despues de la coma. Quitamos un \r por si el
        // archivo fue editado con salto de linea de Windows.
        if (!resto.empty() && resto[resto.size() - 1] == '\r') {
            resto = resto.substr(0, resto.size() - 1);
        }

        if (resto.empty()) {
            ignoradas++;
            continue;
        }

        int edad = atoi(resto.c_str());
        if (edad <= 0) {
            ignoradas++;   // incluye la linea de encabezado (artista,edad)
            continue;
        }

        insertar(nombre, edad);
        cargadas++;
    }

    archivo.close();
    cout << "CSV cargado: " << cargadas << " cancion(es) insertadas, "
         << ignoradas << " linea(s) ignoradas." << endl;
}

// Muestra en consola el contenido de cada cubo, con sus listas encadenadas.
void TablaHash::verTabla() {
    cout << endl;
    cout << "Metodo hash: " << nombreMetodoHash() << " sobre el nombre del artista." << endl;
    for (int i = 0; i < tamano; i++) {
        cout << "[" << i << "] -> ";
        Nodo* actual = cubos[i];
        if (actual == nullptr) {
            cout << "vacio" << endl;
            continue;
        }
        while (actual != nullptr) {
            cout << actual->artista << "(" << actual->edad << ")";
            if (actual->siguiente != nullptr) cout << " -> ";
            actual = actual->siguiente;
        }
        cout << endl;
    }
    cout << "Cubos: " << tamano << " (tamano fijo)" << endl;
    cout << endl;
}

// Genera el .dot de la tabla y lo convierte a imagen con Graphviz. Estilo
// igual al de Spotify: los buckets en una misma fila y las canciones abajo.
bool TablaHash::graficar() {
    string ruta = "TablaHash/tabla_hash.dot";
    FILE* fp = fopen(ruta.c_str(), "w");
    if (fp == nullptr) {
        fprintf(stderr, "Error al abrir el archivo '%s'\n", ruta.c_str());
        return false;
    }

    fprintf(fp, "digraph TablaHash {\n");
    fprintf(fp, "rankdir=TB;\n");
    fprintf(fp, "splines=ortho;\n");
    fprintf(fp, "compound=true;\n");
    fprintf(fp, "bgcolor=\"white\";\n");
    fprintf(fp, "fontname=\"Helvetica\";\n");
    fprintf(fp, "node [shape=box, style=\"rounded,filled\", fontname=\"Helvetica\", fontsize=11];\n");
    fprintf(fp, "edge [color=black, arrowsize=0.8, penwidth=1.2];\n");

    // --- FILA DE CABECERAS DE BUCKETS (como la fila de artistas) ---
    fprintf(fp, "{ rank=same; ");
    for (int i = 0; i < tamano; i++) {
        fprintf(fp, "bucket%d; ", i);
    }
    fprintf(fp, "}\n");

    for (int i = 0; i < tamano; i++) {
        fprintf(fp, "bucket%d [label=\"Bucket %d\", shape=box, style=\"filled,rounded\", fillcolor=\"black\", color=\"black\", penwidth=2, width=1.8, height=0.8, fontname=\"Helvetica-Bold\", fontcolor=white, fontsize=12];\n", i, i);
    }

    // Conectamos las cabeceras en orden para representar el arreglo.
    for (int i = 0; i < tamano - 1; i++) {
        fprintf(fp, "bucket%d -> bucket%d [color=black, penwidth=1.5, arrowhead=vee];\n", i, i + 1);
    }

    // --- CANCIONES DE CADA BUCKET, una debajo de la otra ---
    // Primero encontramos la cadena mas larga para saber cuantas filas hay.
    int maxPorBucket = 0;
    for (int i = 0; i < tamano; i++) {
        int cont = 0;
        Nodo* actual = cubos[i];
        while (actual != nullptr) {
            cont++;
            actual = actual->siguiente;
        }
        if (cont > maxPorBucket) maxPorBucket = cont;
    }

    for (int i = 0; i < tamano; i++) {
        int cont = 0;
        Nodo* actual = cubos[i];
        while (actual != nullptr) {
            string nombre = actual->artista;
            if (nombre.size() > 16) nombre = nombre.substr(0, 15) + "...";
            string mensaje = nombre + "\\n" + to_string(actual->edad) + " anios";
            fprintf(fp, "c%d_%d [label=\"%s\", shape=box, style=\"filled\", fillcolor=\"#1e3ca0\", color=\"black\", penwidth=1.5, width=1.5, height=0.9, fontcolor=white, fontname=\"Helvetica\"];\n",
                i, cont, mensaje.c_str());
            actual = actual->siguiente;
            cont++;
        }
    }

    // Conexion bucket -> primera cancion, y cadena hacia abajo.
    for (int i = 0; i < tamano; i++) {
        int cont = 0;
        Nodo* actual = cubos[i];
        while (actual != nullptr) {
            if (cont == 0) {
                fprintf(fp, "bucket%d -> c%d_0 [color=black, penwidth=2, arrowhead=vee];\n", i, i);
            } else {
                fprintf(fp, "c%d_%d -> c%d_%d [color=black, penwidth=1.5, arrowhead=vee, weight=10];\n",
                    i, cont - 1, i, cont);
            }
            actual = actual->siguiente;
            cont++;
        }
    }

    // --- ALINEACION VERTICAL: la cancion nivel K de cada bucket en la misma fila ---
    // Como en Spotify, donde las canciones del mismo nivel quedan a la misma altura.
    for (int k = 0; k < maxPorBucket; k++) {
        fprintf(fp, "{ rank=same; ");
        for (int i = 0; i < tamano; i++) {
            if (existeEnNivel(i, k)) {
                fprintf(fp, "c%d_%d; ", i, k);
            }
        }
        fprintf(fp, "}\n");
    }

    fprintf(fp, "}\n");
    fclose(fp);

#ifdef _WIN32
    int dotResult = system("dot -Tpng TablaHash/tabla_hash.dot -o TablaHash/tabla_hash.png");
#else
    int dotResult = system("dot -Tpng TablaHash/tabla_hash.dot -o TablaHash/tabla_hash.png 2>/dev/null");
#endif

    if (dotResult != 0) {
        cout << "Se genero el archivo tabla_hash.dot." << endl;
        cout << "No se pudo generar el PNG (Graphviz no disponible)." << endl;
        return true;
    }

#ifdef _WIN32
    system("start TablaHash/tabla_hash.png");
#else
    cout << "Imagen generada en TablaHash/tabla_hash.png" << endl;
#endif

    return true;
}

// Menu principal: deja probar las operaciones basicas de la tabla hash.
int main() {
    // La tabla hash es de TAMAÑO FIJO: 13 buckets. No cambia durante la ejecucion.
    const int TAM_FIJO = 13;
    TablaHash tabla(TAM_FIJO);
    int opcion;

    do {
        cout << endl;
        cout << "========== M E N U ===========" << endl;
        cout << "1. Insertar cancion (artista y edad)" << endl;
        cout << "2. Buscar canciones por artista" << endl;
        cout << "3. Eliminar cancion por artista" << endl;
        cout << "4. Ver tabla" << endl;
        cout << "5. Graficar tabla" << endl;
        cout << "6. Cargar canciones desde CSV" << endl;
        cout << "7. Salir" << endl;
        cout << "Ingrese una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        // Pide artista y edad, y guarda la cancion usando el NOMBRE del
        // artista como clave para calcular el indice.
        case 1: {
            string artista;
            int edad;
            cout << "Nombre del artista: ";
            getline(cin, artista);
            cout << "Edad del artista (anios): ";
            cin >> edad;
            cin.ignore();
            tabla.insertar(artista, edad);
            cout << "Cancion de \"" << artista << "\" insertada en el indice "
                 << tabla.hash(artista) << "." << endl;
            break;
        }
        case 2: {
            string artista;
            cout << "Artista a buscar: ";
            getline(cin, artista);
            tabla.buscar(artista);
            break;
        }
        case 3: {
            string artista;
            cout << "Artista a eliminar: ";
            getline(cin, artista);
            tabla.eliminar(artista);
            break;
        }
        case 4:
            tabla.verTabla();
            break;
        case 5:
            tabla.graficar();
            break;
        // Pide la ruta del CSV e inserta todas las canciones del archivo.
        // El proyecto incluye un ejemplo: TablaHash/canciones.csv
        case 6: {
            string ruta;
            cout << "Ruta del archivo CSV (ej. TablaHash/canciones.csv): ";
            getline(cin, ruta);
            if (ruta.empty()) ruta = "TablaHash/canciones.csv";
            tabla.cargarCSV(ruta);
            break;
        }
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
