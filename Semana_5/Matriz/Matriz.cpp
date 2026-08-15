#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>

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

// Node es la celda de la matriz dispersa, guarda la fila, la columna y el valor
// Cada celda tiene punteros a sus cuatro vecinos, arriba, abajo, izquierda y derecha
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

// MatrizDispersaTipoDirector maneja la matriz de peliculas
// Las filas son los tipos de pelicula, las columnas los directores y el valor la pelicula
class MatrizDispersaTipoDirector {
public:

    MatrizDispersaTipoDirector();


    ~MatrizDispersaTipoDirector();


    Node* createNode(string row, string col, string val);

    Node* insertarTipo(const string& tipo);

    Node* insertarDirector(const string& director);

    void printTipoAndDirectorByPelicula(const string& pelicula);

    void cargarCSV(const string& ruta);

    bool ObtenerGraphviz();

    void eliminarPelicula(const string& pelicula);

private:
    Node* head;


    Node* createVertHead(string row);


    Node* createHorzHead(string col);


    Node* searchHorzHead(string col);


    Node* searchVertHead(string row);


    void deleteAllNodes(Node* nodo);

    string replaceSpaces(const string& str);

};


// crea la matriz con un nodo cabecera vacio que enlaza filas y columnas
MatrizDispersaTipoDirector::MatrizDispersaTipoDirector() {
    head = new Node();
}

// libera la memoria de todos los nodos de la matriz
MatrizDispersaTipoDirector::~MatrizDispersaTipoDirector() {
    deleteAllNodes(head);
}

// elimina todos los nodos de la matriz
// cada celda pertenece a una sola fila, por lo que se recorre la lista de
// celdas de cada fila y se libera cada celda exactamente una vez
void MatrizDispersaTipoDirector::deleteAllNodes(Node* nodo) {
    if (nodo == nullptr) return;

    // se liberan los encabezados de fila y sus celdas
    Node* row = nodo->down;
    while (row != nullptr) {
        Node* siguienteFila = row->down;
        Node* celda = row->right;
        while (celda != nullptr) {
            Node* siguienteCelda = celda->right;
            delete celda;
            celda = siguienteCelda;
        }
        delete row;
        row = siguienteFila;
    }

    // se liberan los encabezados de columna
    Node* col = nodo->right;
    while (col != nullptr) {
        Node* siguienteCol = col->right;
        delete col;
        col = siguienteCol;
    }

    delete nodo;
}


// reemplaza los espacios por guiones bajos para los nombres de los nodos en el grafo
string MatrizDispersaTipoDirector::replaceSpaces(const string& str) {
    string result = str;
    for (size_t i = 0; i < result.size(); i++) {
        if (result[i] == ' ') {
            result[i] = '_';
        }
    }
    return result;
}


// busca una pelicula por nombre y la desenlaza de la matriz
void MatrizDispersaTipoDirector::eliminarPelicula(const string& pelicula) {
    Node* rowHead = head->down;
    Node* targetNode = nullptr;

    // recorremos todas las filas y columnas buscando la pelicula
    while (rowHead != nullptr && targetNode == nullptr) {
        Node* colNode = rowHead->right;
        while (colNode != nullptr) {
            if (colNode->val == pelicula) {
                targetNode = colNode;
                break;
            }
            colNode = colNode->right;
        }
        rowHead = rowHead->down;
    }


    // si la pelicula no existe, termina sin hacer cambios
    if (targetNode == nullptr) {
        cout << "Pelicula no encontrada." << endl;
        return;
    }

    // desconectamos la celda de sus cuatro vecinos
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

    // los encabezados de tipo y director se conservan aunque queden vacios

    delete targetNode;
    cout << "La pelicula '" << pelicula << "' fue eliminada de la matriz." << endl;
}


// carga las peliculas desde un archivo CSV con formato tipo,director,pelicula
void MatrizDispersaTipoDirector::cargarCSV(const string& ruta) {
    ifstream archivo(ruta);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo '" << ruta << "'." << endl;
        return;
    }

    string linea;
    bool encabezado = true;
    int insertadas = 0;

    while (getline(archivo, linea)) {
        if (linea.empty()) {
            continue;
        }

        // la primera linea se considera el encabezado y se ignora
        if (encabezado) {
            encabezado = false;
            continue;
        }

        // separamos los campos por comas y quitamos las comillas dobles si las hay
        size_t inicio = 0;
        ArregloDinamico<string> campos;
        for (size_t i = 0; i <= linea.size(); ++i) {
            if (i == linea.size() || linea[i] == ',') {
                string campo = linea.substr(inicio, i - inicio);
                if (campo.size() >= 2 && campo.front() == '"' && campo.back() == '"') {
                    campo = campo.substr(1, campo.size() - 2);
                }
                campos.agregar(campo);
                inicio = i + 1;
            }
        }

        if (campos.size() >= 3) {
            createNode(campos[0], campos[1], campos[2]);
            insertadas++;
        }
    }

    archivo.close();
    cout << "Se cargaron " << insertadas << " peliculas desde '" << ruta << "'." << endl;
}





// inserta una pelicula en la posicion (tipo, director) creando los encabezados que falten
Node* MatrizDispersaTipoDirector::createNode(string row, string col, string val) {
    Node* newNode = new Node(row, replaceSpaces(col), val);

    // creamos o recuperamos los encabezados de fila y columna
    Node* temp = createVertHead(row);
    Node* temp2 = createHorzHead(replaceSpaces(col));

    Node* temp3 = temp->right;
    Node* temp4 = temp2->down;

    // buscamos la posicion de la nueva celda dentro de la fila
    while (temp3 != nullptr) {
        if (temp3->col > col) {
            break;
        }
        temp = temp->right;
        temp3 = temp3->right;
    }

    // buscamos la posicion de la nueva celda dentro de la columna
    while (temp4 != nullptr) {
        if (temp4->row > row) {
            break;
        }
        temp2 = temp2->down;
        temp4 = temp4->down;
    }

    // enlazamos la nueva celda con sus vecinos
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

// crea el encabezado de fila para un tipo de pelicula
Node* MatrizDispersaTipoDirector::insertarTipo(const string& tipo) {
    return createVertHead(tipo);
}

// crea el encabezado de columna para un director
Node* MatrizDispersaTipoDirector::insertarDirector(const string& director) {
    return createHorzHead(replaceSpaces(director));
}

// busca un encabezado de columna por su nombre
Node* MatrizDispersaTipoDirector::searchHorzHead(string col) {
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->col == col) {
            return temp;
        }
        temp = temp->right;
    }
    return nullptr;
}

// crea un encabezado de columna y lo inserta ordenado, si ya existe lo devuelve
Node* MatrizDispersaTipoDirector::createHorzHead(string col) {
    Node* newNode = searchHorzHead(col);
    if (newNode != nullptr) {
        return newNode;
    }

    newNode = new Node("", col, "");

    Node* temp = head;

    // buscamos la posicion del nuevo encabezado dentro de los directores
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


// busca un encabezado de fila por su nombre
Node* MatrizDispersaTipoDirector::searchVertHead(string row) {
    Node* temp = head;
    while (temp != nullptr) {
        if (temp->row == row) {
            return temp;
        }
        temp = temp->down;
    }
    return nullptr;
}


// crea un encabezado de fila y lo inserta ordenado, si ya existe lo devuelve
Node* MatrizDispersaTipoDirector::createVertHead(string row) {
    Node* newNode = searchVertHead(row);
    if (newNode != nullptr) {
        return newNode;
    }

    newNode = new Node(row, "", "");

    Node* temp = head;

    // buscamos la posicion del nuevo encabezado dentro de los tipos
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



// busca una pelicula por nombre e imprime su tipo y su director
void MatrizDispersaTipoDirector::printTipoAndDirectorByPelicula(const string& pelicula) {
    Node* rowHead = head->down;
    while (rowHead != nullptr) {
        Node* colNode = rowHead->right;
        while (colNode != nullptr) {
            if (colNode->val == pelicula) {
                cout << "Tipo: " << colNode->row << ", Director: " << colNode->col << endl;
                return;
            }
            colNode = colNode->right;
        }
        rowHead = rowHead->down;
    }
    cout << "Pelicula no encontrada." << endl;
}

// genera el archivo .dot de la matriz y lo compila a PNG con Graphviz
bool MatrizDispersaTipoDirector::ObtenerGraphviz() {
    FILE* fp = fopen("Matriz/MatrizDispersa.dot", "w");
    if (fp == nullptr) {
        fprintf(stderr, "Error al abrir el archivo 'Matriz/MatrizDispersa.dot'\n");
        return false;
    }

    fprintf(fp, "digraph Sparce_Matrix {\n");
    fprintf(fp, "rankdir=TB;\n");
    fprintf(fp, "splines=ortho;\n");
    fprintf(fp, "bgcolor=\"#f7f9fc\";\n");
    fprintf(fp, "pad=0.6;\n");
    fprintf(fp, "nodesep=0.6;\n");
    fprintf(fp, "ranksep=0.9;\n");
    fprintf(fp, "label=\"Matriz Dispersa Tipo ** Director\";\n");
    fprintf(fp, "labelloc=t;\n");
    fprintf(fp, "fontname=\"Arial\";\n");
    fprintf(fp, "node [shape=box, style=\"rounded,filled\", fontname=\"Arial\", fontsize=11, color=\"#2c3e50\"];\n");
    fprintf(fp, "edge [color=\"#b0bec5\", arrowsize=0.8, penwidth=1.2];\n");

    // creamos el nodo central que representa la interseccion de tipos y directores
    fprintf(fp, "Mt [label=\"TIPO ** DIRECTOR\", width=1.5, height=0.6, style=\"rounded,filled\", fillcolor=\"#34495e\", fontcolor=white, fontsize=13, group=0, color=\"#1b2631\", penwidth=2];\n");

    Node* temp = head->right;

    // los encabezados de los directores quedan al mismo nivel que el nodo central
    fprintf(fp, "{ rank=same; Mt; ");
    temp = head->right;
    while (temp != nullptr) {
        fprintf(fp, "Node%s; ", replaceSpaces(temp->col).c_str());
        temp = temp->right;
    }
    fprintf(fp, "}\n");

    // creamos los nodos de los directores
    int group = 1;
    temp = head->right;
    while (temp != nullptr) {
        fprintf(fp, "Node%s [label=\"%s\", style=\"rounded,filled\", fillcolor=\"#e67e22\", fontcolor=white, fontsize=12, group=%d, color=\"#d35400\", penwidth=1.5];\n",
            replaceSpaces(temp->col).c_str(), replaceSpaces(temp->col).c_str(), group);
        group++;
        temp = temp->right;
    }

    // creamos los nodos de los tipos de pelicula
    temp = head->down;
    while (temp != nullptr) {
        fprintf(fp, "Node%s [label=\"%s\", style=\"rounded,filled\", fillcolor=\"#16a085\", fontcolor=white, fontsize=12, group=0, color=\"#117a65\", penwidth=1.5];\n",
            replaceSpaces(temp->row).c_str(), replaceSpaces(temp->row).c_str());
        temp = temp->down;
    }

    // paleta de colores para las peliculas, alterna segun el director
    const char* paleta[] = { "#aed6f1", "#a9dfbf", "#f9e79f", "#f5b7b1", "#d7bde2", "#a3e4d7", "#fadbd8", "#d5f5e3" };

    // creamos los nodos de las peliculas con el grupo de su director
    temp = head->down;
    while (temp != nullptr) {
        Node* temp2 = temp->right;
        while (temp2 != nullptr) {
            int colGroup = 1;
            Node* header = head->right;
            while (header != nullptr && replaceSpaces(header->col) != replaceSpaces(temp2->col)) {
                header = header->right;
                colGroup++;
            }
            fprintf(fp, "Node%s%s [label=\"%s\", style=\"rounded,filled\", fillcolor=\"%s\", fontcolor=\"#1b2631\", group=%d, color=\"#5d6d7e\"];\n",
                replaceSpaces(temp2->row).c_str(), replaceSpaces(temp2->col).c_str(),
                replaceSpaces(temp2->val).c_str(), paleta[(colGroup - 1) % 8], colGroup);
            temp2 = temp2->right;
        }
        temp = temp->down;
    }

    // cada fila de la matriz queda en el mismo nivel para que se vea ordenada
    temp = head->down;
    while (temp != nullptr) {
        Node* temp2 = temp->right;
        fprintf(fp, "{ rank=same; Node%s; ", replaceSpaces(temp->row).c_str());
        while (temp2 != nullptr) {
            fprintf(fp, "Node%s%s; ", replaceSpaces(temp2->row).c_str(), replaceSpaces(temp2->col).c_str());
            temp2 = temp2->right;
        }
        fprintf(fp, "}\n");
        temp = temp->down;
    }

    // contamos el total de peliculas, directores y tipos
    int totalPeliculas = 0;
    int totalDirectores = 0;
    int totalTipos = 0;

    temp = head->right;
    while (temp != nullptr) {
        totalDirectores++;
        temp = temp->right;
    }

    temp = head->down;
    while (temp != nullptr) {
        totalTipos++;
        temp = temp->down;
    }

    temp = head->down;
    while (temp != nullptr) {
        Node* temp2 = temp->right;
        while (temp2 != nullptr) {
            totalPeliculas++;
            temp2 = temp2->right;
        }
        temp = temp->down;
    }

    // resumen estadistico que se muestra como una tabla
    fprintf(fp, "stats [label=<<TABLE BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"5\" COLOR=\"#5d6d7e\" BGCOLOR=\"#eaf2f8\">\n");
    fprintf(fp, "<TR><TD COLSPAN=\"2\" BGCOLOR=\"#34495e\" ALIGN=\"CENTER\"><FONT COLOR=\"white\" FACE=\"Arial\"><B>ESTADISTICAS</B></FONT></TD></TR>\n");
    fprintf(fp, "<TR><TD ALIGN=\"LEFT\"><FONT FACE=\"Arial\">Total peliculas</FONT></TD><TD ALIGN=\"RIGHT\"><FONT FACE=\"Arial\"><B>%d</B></FONT></TD></TR>\n", totalPeliculas);
    fprintf(fp, "<TR><TD ALIGN=\"LEFT\"><FONT FACE=\"Arial\">Total directores</FONT></TD><TD ALIGN=\"RIGHT\"><FONT FACE=\"Arial\"><B>%d</B></FONT></TD></TR>\n", totalDirectores);
    fprintf(fp, "<TR><TD ALIGN=\"LEFT\"><FONT FACE=\"Arial\">Total tipos</FONT></TD><TD ALIGN=\"RIGHT\"><FONT FACE=\"Arial\"><B>%d</B></FONT></TD></TR>\n", totalTipos);
    fprintf(fp, "<TR><TD COLSPAN=\"2\" BGCOLOR=\"#d4e6f1\" ALIGN=\"CENTER\"><FONT FACE=\"Arial\"><B>Peliculas por tipo</B></FONT></TD></TR>\n");

    temp = head->down;
    while (temp != nullptr) {
        int count = 0;
        Node* temp2 = temp->right;
        while (temp2 != nullptr) {
            count++;
            temp2 = temp2->right;
        }
        fprintf(fp, "<TR><TD ALIGN=\"LEFT\" COLSPAN=\"2\"><FONT FACE=\"Arial\">%s: <B>%d</B></FONT></TD></TR>\n",
            replaceSpaces(temp->row).c_str(), count);
        temp = temp->down;
    }

    fprintf(fp, "<TR><TD COLSPAN=\"2\" BGCOLOR=\"#fdebd0\" ALIGN=\"CENTER\"><FONT FACE=\"Arial\"><B>Peliculas por director</B></FONT></TD></TR>\n");

    temp = head->right;
    while (temp != nullptr) {
        int count = 0;
        Node* temp2 = temp->down;
        while (temp2 != nullptr) {
            count++;
            temp2 = temp2->down;
        }
        fprintf(fp, "<TR><TD ALIGN=\"LEFT\" COLSPAN=\"2\"><FONT FACE=\"Arial\">%s: <B>%d</B></FONT></TD></TR>\n",
            replaceSpaces(temp->col).c_str(), count);
        temp = temp->right;
    }

    fprintf(fp, "</TABLE>>, shape=plain];\n");

    temp = head->down;
    int filaCentral = 0;
    while (temp != nullptr && filaCentral < (totalTipos + 1) / 2 - 1) {
        temp = temp->down;
        filaCentral++;
    }
    if (temp != nullptr) {
        fprintf(fp, "{ rank=same; Node%s; stats; }\n", replaceSpaces(temp->row).c_str());
    } else {
        fprintf(fp, "{ rank=same; Mt; stats; }\n");
    }

    // el nodo central se conecta con el primer tipo y el primer director
    if (head->down != nullptr) {
        fprintf(fp, "Mt -> Node%s [color=\"#34495e\", penwidth=2, arrowhead=normal];\n", replaceSpaces(head->down->row).c_str());
    }
    if (head->right != nullptr) {
        fprintf(fp, "Mt -> Node%s [color=\"#34495e\", penwidth=2, arrowhead=normal];\n", replaceSpaces(head->right->col).c_str());
    }

    // flechas que conectan los tipos de pelicula entre si
    Node* anterior = head;
    temp = head->down;
    while (temp != nullptr) {
        if (anterior != head) {
            fprintf(fp, "Node%s -> Node%s [color=\"#16a085\", penwidth=2, arrowhead=normal];\n",
                replaceSpaces(anterior->row).c_str(), replaceSpaces(temp->row).c_str());
        }
        anterior = temp;
        temp = temp->down;
    }

    // flechas que conectan los directores entre si
    anterior = head;
    temp = head->right;
    while (temp != nullptr) {
        if (anterior != head) {
            fprintf(fp, "Node%s -> Node%s [color=\"#e67e22\", penwidth=2, arrowhead=normal];\n",
                replaceSpaces(anterior->col).c_str(), replaceSpaces(temp->col).c_str());
        }
        anterior = temp;
        temp = temp->right;
    }

    // cada tipo de pelicula se conecta con sus peliculas y estas se encadenan entre si
    temp = head->down;
    while (temp != nullptr) {
        Node* actual = temp->right;
        if (actual != nullptr) {
            fprintf(fp, "Node%s -> Node%s%s [color=\"#5dade2\", penwidth=1.5, arrowhead=normal];\n",
                replaceSpaces(temp->row).c_str(), replaceSpaces(actual->row).c_str(), replaceSpaces(actual->col).c_str());
            fprintf(fp, "Node%s%s -> Node%s [color=\"#5dade2\", penwidth=1.5, arrowhead=normal];\n",
                replaceSpaces(actual->row).c_str(), replaceSpaces(actual->col).c_str(), replaceSpaces(temp->row).c_str());
            while (actual->right != nullptr) {
                Node* siguiente = actual->right;
                fprintf(fp, "Node%s%s -> Node%s%s [color=\"#5dade2\", penwidth=1.5, arrowhead=normal];\n",
                    replaceSpaces(actual->row).c_str(), replaceSpaces(actual->col).c_str(),
                    replaceSpaces(siguiente->row).c_str(), replaceSpaces(siguiente->col).c_str());
                fprintf(fp, "Node%s%s -> Node%s%s [color=\"#5dade2\", penwidth=1.5, arrowhead=normal];\n",
                    replaceSpaces(siguiente->row).c_str(), replaceSpaces(siguiente->col).c_str(),
                    replaceSpaces(actual->row).c_str(), replaceSpaces(actual->col).c_str());
                actual = siguiente;
            }
        }
        temp = temp->down;
    }

    // cada director se conecta con sus peliculas y estas se encadenan entre si
    temp = head->right;
    while (temp != nullptr) {
        Node* actual = temp->down;
        if (actual != nullptr) {
            fprintf(fp, "Node%s -> Node%s%s [color=\"#eb984e\", penwidth=1.5, arrowhead=normal];\n",
                replaceSpaces(temp->col).c_str(), replaceSpaces(actual->row).c_str(), replaceSpaces(actual->col).c_str());
            fprintf(fp, "Node%s%s -> Node%s [color=\"#eb984e\", penwidth=1.5, arrowhead=normal];\n",
                replaceSpaces(actual->row).c_str(), replaceSpaces(actual->col).c_str(), replaceSpaces(temp->col).c_str());
            while (actual->down != nullptr) {
                Node* siguiente = actual->down;
                fprintf(fp, "Node%s%s -> Node%s%s [color=\"#eb984e\", penwidth=1.5, arrowhead=normal];\n",
                    replaceSpaces(actual->row).c_str(), replaceSpaces(actual->col).c_str(),
                    replaceSpaces(siguiente->row).c_str(), replaceSpaces(siguiente->col).c_str());
                fprintf(fp, "Node%s%s -> Node%s%s [color=\"#eb984e\", penwidth=1.5, arrowhead=normal];\n",
                    replaceSpaces(siguiente->row).c_str(), replaceSpaces(siguiente->col).c_str(),
                    replaceSpaces(actual->row).c_str(), replaceSpaces(actual->col).c_str());
                actual = siguiente;
            }
        }
        temp = temp->right;
    }

    fprintf(fp, "}\n");

    fclose(fp);

    // compilamos el .dot a PNG con Graphviz (si esta instalado)
#ifdef _WIN32
    int dotResult = system("dot -Tpng Matriz/MatrizDispersa.dot -o Matriz/MatrizDispersa.png");
#else
    int dotResult = system("dot -Tpng Matriz/MatrizDispersa.dot -o Matriz/MatrizDispersa.png 2>/dev/null");
#endif
    if (dotResult != 0) {
        cout << "Se genero el archivo MatrizDispersa.dot." << endl;
        cout << "No se pudo generar el PNG (Graphviz no disponible)." << endl;
        cout << "Instalalo y ejecuta: dot -Tpng MatrizDispersa.dot -o MatrizDispersa.png" << endl;
        return true;
    }

#ifdef _WIN32
    // en Windows abrimos la imagen con el visor predeterminado
    system("start Matriz/MatrizDispersa.png");
#else
    // en Linux no se abre un visor, solo se indica donde quedo el PNG
    cout << "Imagen generada en Matriz/MatrizDispersa.png" << endl;
#endif
    return true;
}

// menu principal del programa
int main() {
    MatrizDispersaTipoDirector matriz;
    int opcion;

    do {
        cout << endl;
        cout << "========== M E N U ===========" << endl;
        cout << "1. Insertar director" << endl;
        cout << "2. Insertar tipo" << endl;
        cout << "3. Insertar pelicula" << endl;
        cout << "4. Eliminar pelicula" << endl;
        cout << "5. Graficar Matriz" << endl;
        cout << "6. Cargar CSV" << endl;
        cout << "7. Salir" << endl;
        cout << "Ingrese una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        // pide el director y crea su encabezado de columna
        case 1: {
            string director;
            cout << "Ingrese el director: ";
            getline(cin, director);

            matriz.insertarDirector(director);
            cout << "Director insertado correctamente." << endl;
            break;
        }
        // pide el tipo de pelicula y crea su encabezado de fila
        case 2: {
            string tipo;
            cout << "Ingrese el tipo de pelicula: ";
            getline(cin, tipo);

            matriz.insertarTipo(tipo);
            cout << "Tipo insertado correctamente." << endl;
            break;
        }
        // pide director, tipo y pelicula e inserta la celda en la matriz
        case 3: {
            string director, tipo, pelicula;
            cout << "Ingrese el director: ";
            getline(cin, director);
            cout << "Ingrese el tipo de pelicula: ";
            getline(cin, tipo);
            cout << "Ingrese el nombre de la pelicula: ";
            getline(cin, pelicula);

            matriz.createNode(tipo, director, pelicula);
            cout << "Pelicula insertada correctamente." << endl;
            break;
        }
        // pide el nombre de la pelicula y la elimina de la matriz
        case 4: {
            string pelicula;
            cout << "Ingrese el nombre de la pelicula a eliminar: ";
            getline(cin, pelicula);

            matriz.eliminarPelicula(pelicula);
            break;
        }
        // genera el reporte grafico de la matriz
        case 5:
            matriz.ObtenerGraphviz();
            break;
        // carga las peliculas desde un archivo CSV
        case 6: {
            string ruta;
            cout << "Ingrese la ruta del archivo CSV: ";
            getline(cin, ruta);

            matriz.cargarCSV(ruta);
            break;
        }
        // termina el programa
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
