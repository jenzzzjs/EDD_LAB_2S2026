#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>

using namespace std;

// Esta clase representa cada celda del mapa de asientos.
// La idea es que cada nodo sepa conectarse con quien tiene arriba, abajo, a la izquierda y a la derecha,
// como una grilla donde cada posicion conoce a sus vecinos.
class Node {
public:
    string row;
    string col;
    string val;
    Node* up;
    Node* down;
    Node* left;
    Node* right;

    // Al crear un nodo nuevo todos sus vecinos arrancan en nullptr porque
    // aun no sabemos a quien tendra alrededor, eso se define despues cuando lo enlazamos.
    Node(string r = "", string c = "", string v = "")
        : row(r), col(c), val(v), up(nullptr), down(nullptr), left(nullptr), right(nullptr) {}
};

// Esta clase es la que maneja todo el mapa de asientos.
// A diferencia de la matriz de la semana 5 que solo creaba nodos cuando habia datos,
// aca al momento de crear el mapa se generan TODOS los nodos de la grilla,
// porque un mapa de asientos necesita mostrar cada posicion posible, vacia o no.
class MapaAsientos {
public:
    MapaAsientos();
    ~MapaAsientos();

    void crearMapa(int filas, int columnas);
    void verMapa();
    bool graficar();

private:
    Node* head;
    int totalFilas;
    int totalColumnas;

    // Esta funcion se encarga de liberar toda la memoria que se uso.
    // Recorre cada fila y va eliminando las celdas una por una,
    // despues elimina los encabezados de columna y por ultimo el head.
    void deleteAllNodes(Node* nodo);

    // Busca el encabezado de una columna recorriendo la fila de encabezados que sale del head hacia la derecha.
    Node* searchHorzHead(string col);

    // Busca el encabezado de una fila recorriendo la columna de encabezados que sale del head hacia abajo.
    Node* searchVertHead(string row);

    // Crea el encabezado de una columna nueva. Si ya existe devuelve el que ya esta.
    // Si no existe lo inserta en orden para que las columnas queden ordenadas de menor a mayor.
    Node* createHorzHead(string col);

    // Igual que createHorzHead pero para las filas, inserta en orden vertical.
    Node* createVertHead(string row);

    // Busca un nodo especifico dentro de la matriz usando su fila y columna.
    Node* findNode(string row, string col);

    // Convierte un numero entero a string sin usar la biblioteca estandar.
    string intToString(int num);
};

// El constructor arranca sin head porque aun no se creo ningun mapa.
MapaAsientos::MapaAsientos() : head(nullptr), totalFilas(0), totalColumnas(0) {}

// El destructor se asegura de que no quede memoria sin liberar cuando el programa termina.
MapaAsientos::~MapaAsientos() {
    if (head != nullptr) {
        deleteAllNodes(head);
    }
}

// Esta funcion es como un "limpie todo" de la matriz.
// Primero recorre cada fila y elimina todas las celdas que tiene,
// despues elimina los encabezados de columna que van hacia la derecha del head,
// y por ultimo elimina el head mismo.
// Es importante guardar el siguiente antes de borrar el actual porque una vez que borramos un nodo
// ya no podemos acceder a sus punteros.
void MapaAsientos::deleteAllNodes(Node* nodo) {
    if (nodo == nullptr) return;

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

    Node* col = nodo->right;
    while (col != nullptr) {
        Node* siguienteCol = col->right;
        delete col;
        col = siguienteCol;
    }

    delete nodo;
}

// Convierte un entero a su representacion como texto.
// Va sacando el ultimo digito con el modulo y lo va concatenando al inicio del resultado.
// Por ejemplo 123 -> "3" + "2" + "1" -> "123".
string MapaAsientos::intToString(int num) {
    string resultado = "";
    if (num == 0) return "0";
    while (num > 0) {
        resultado = char('0' + (num % 10)) + resultado;
        num /= 10;
    }
    return resultado;
}

// Recorre los encabezados de columna que van hacia la derecha del head
// hasta encontrar uno cuyo nombre coincida con el que estamos buscando.
// Si no lo encuentra retorna nullptr.
Node* MapaAsientos::searchHorzHead(string col) {
    if (head == nullptr) return nullptr;
    Node* temp = head->right;
    while (temp != nullptr) {
        if (temp->col == col) return temp;
        temp = temp->right;
    }
    return nullptr;
}

// Igual que searchHorzHead pero recorre hacia abajo buscando el encabezado de fila.
Node* MapaAsientos::searchVertHead(string row) {
    if (head == nullptr) return nullptr;
    Node* temp = head->down;
    while (temp != nullptr) {
        if (temp->row == row) return temp;
        temp = temp->down;
    }
    return nullptr;
}

// Crea un encabezado de columna nuevo y lo pone en su lugar correcto.
// Primero verifica si ya existe para no duplicar.
// Despues busca la posicion donde le toca segun el orden alfabetico/numerico
// y lo enlaza entre sus vecinos.
// Si es el primer encabezado que se crea, tambien crea el head de la matriz.
Node* MapaAsientos::createHorzHead(string col) {
    Node* existente = searchHorzHead(col);
    if (existente != nullptr) return existente;

    Node* newNode = new Node("", col, "");

    if (head == nullptr) {
        head = new Node();
        head->right = newNode;
        newNode->left = head;
        return newNode;
    }

    Node* temp = head;
    while (temp->right != nullptr) {
        if (temp->right->col > col) break;
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

// Mismo criterio que createHorzHead pero para los encabezados de fila.
// Estos van hacia abajo del head y se insertan en orden vertical.
Node* MapaAsientos::createVertHead(string row) {
    Node* existente = searchVertHead(row);
    if (existente != nullptr) return existente;

    Node* newNode = new Node(row, "", "");

    if (head == nullptr) {
        head = new Node();
        head->down = newNode;
        newNode->up = head;
        return newNode;
    }

    Node* temp = head;
    while (temp->down != nullptr) {
        if (temp->down->row > row) break;
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

// Busca un nodo dentro de la matriz por su fila y columna.
// Primero busca el encabezado de fila y despues recorre sus hijos hacia la derecha
// hasta encontrar la columna que coincide.
Node* MapaAsientos::findNode(string row, string col) {
    Node* vertHead = searchVertHead(row);
    if (vertHead == nullptr) return nullptr;
    Node* temp = vertHead->right;
    while (temp != nullptr) {
        if (temp->col == col) return temp;
        temp = temp->right;
    }
    return nullptr;
}

// Esta es la funcion principal que construye el mapa de asientos.
// Lo que hace es crear la grilla completa con todos sus nodos internos,
// lo cual es diferente a la matriz dispersa tipica donde solo se crean nodos cuando hay datos.
// Aca cada posicion de la grilla tiene su nodo aunque este vacia,
// porque al graficar queremos ver todos los asientos posibles.
void MapaAsientos::crearMapa(int filas, int columnas) {
    // Si ya existia un mapa lo borramos primero para empezar de cero.
    if (head != nullptr) {
        deleteAllNodes(head);
        head = nullptr;
    }

    totalFilas = filas;
    totalColumnas = columnas;

    // Primero creamos todos los encabezados de columna.
    // Estos son los que van en la parte superior del mapa numerados como 1, 2, 3...
    for (int c = 1; c <= columnas; c++) {
        createHorzHead(intToString(c));
    }

    // Luego creamos los encabezados de fila que van en el lado izquierdo.
    for (int f = 1; f <= filas; f++) {
        createVertHead(intToString(f));
    }

    // Ahora viene lo interesante: crear cada celda de la grilla.
    // Recorremos todas las combinaciones posibles de fila y columna
    // y por cada una creamos un nodo con valor "---" que significa asiento vacio.
    // Cada nodo se enlaza tanto en su fila como en su columna.
    for (int f = 1; f <= filas; f++) {
        for (int c = 1; c <= columnas; c++) {
            string r = intToString(f);
            string col = intToString(c);

            Node* newNode = new Node(r, col, "---");

            Node* vertHead = searchVertHead(r);
            Node* horzHead = searchVertHead(col);

            // Buscamos donde insertar el nodo dentro de su fila.
            // Vamos recorriendo los vecinos de la derecha hasta encontrar uno que sea mayor,
            // y ahi es donde enlazamos el nuevo nodo.
            Node* tempRow = vertHead;
            Node* tempRowNext = vertHead->right;
            while (tempRowNext != nullptr) {
                if (tempRowNext->col > col) break;
                tempRow = tempRowNext;
                tempRowNext = tempRowNext->right;
            }

            // Hacemos lo mismo pero para la columna, buscamos donde le toca arriba y abajo.
            Node* horzH = searchHorzHead(col);
            Node* tempCol = horzH;
            Node* tempColNext = horzH->down;
            while (tempColNext != nullptr) {
                if (tempColNext->row > r) break;
                tempCol = tempColNext;
                tempColNext = tempColNext->down;
            }

            // Enlazamos el nodo con sus cuatro vecinos.
            // Si algun vecino es nullptr significa que es el extremo de la fila o columna.
            newNode->right = tempRowNext;
            newNode->left = tempRow;
            newNode->up = tempCol;
            newNode->down = tempColNext;

            if (tempRowNext != nullptr) tempRowNext->left = newNode;
            if (tempColNext != nullptr) tempColNext->up = newNode;
            tempRow->right = newNode;
            tempCol->down = newNode;
        }
    }

    cout << "Mapa de asientos creado: " << filas << " filas x " << columnas << " columnas" << endl;
}

// Esta funcion muestra el mapa de asientos en la consola.
// Recorre la matriz de arriba hacia abajo y de izquierda a derecha,
// imprimiendo cada celda con su valor.
// Los asientos vacios se muestran como "--" y los ocupados con el nombre de la persona.
void MapaAsientos::verMapa() {
    if (head == nullptr) {
        cout << "No hay mapa creado. Use la opcion 1 primero." << endl;
        return;
    }

    cout << endl;

    // Imprimimos los numeros de columna como encabezado.
    cout << "       ";
    for (int c = 1; c <= totalColumnas; c++) {
        string col = intToString(c);
        if (col.size() < 4) {
            for (size_t i = col.size(); i < 4; i++) cout << " ";
        }
        cout << col << "  ";
    }
    cout << endl;

    // Linea separadora.
    cout << "      +";
    for (int c = 1; c <= totalColumnas; c++) {
        cout << "------+";
    }
    cout << endl;

    // Recorremos cada fila desde el primer encabezado de fila hacia abajo.
    Node* rowHead = head->down;
    while (rowHead != nullptr) {
        // Imprimimos el numero de fila a la izquierda.
        string filaLabel = "F" + rowHead->row;
        if (filaLabel.size() < 6) {
            for (size_t i = filaLabel.size(); i < 6; i++) cout << " ";
        }
        cout << filaLabel << "|";

        // Ahora recorremos las celdas de esta fila de izquierda a derecha.
        // Usamos colActual para saber que columna deberia venir,
        // porque puede haber celdas que no esten enlazadas si la matriz fuera dispersa.
        Node* cell = rowHead->right;
        int colActual = 1;
        while (cell != nullptr || colActual <= totalColumnas) {
            if (cell != nullptr && cell->col == intToString(colActual)) {
                if (cell->val == "---") {
                    cout << "  --  |";
                } else {
                    string nombre = cell->val;
                    if (nombre.size() > 5) nombre = nombre.substr(0, 5);
                    for (size_t i = nombre.size(); i < 5; i++) nombre = " " + nombre;
                    cout << nombre << " |";
                }
                cell = cell->right;
            } else {
                cout << "      |";
            }
            colActual++;
        }
        cout << endl;

        cout << "      +";
        for (int c = 1; c <= totalColumnas; c++) {
            cout << "------+";
        }
        cout << endl;

        rowHead = rowHead->down;
    }

    cout << endl;
    cout << "Leyenda: -- = disponible" << endl;
    cout << endl;
}

// Esta funcion genera el archivo .dot con Graphviz y lo convierte a imagen.
// La particularidad de esta matriz frente a la de la semana 5 es que aca se grafican
// TODOS los nodos internos sin importar si estan vacios o no,
// porque un mapa de asientos tiene que mostrar cada posicion posible.
// Los asientos vacios se pintan de gris y los ocupados de verde.
bool MapaAsientos::graficar() {
    if (head == nullptr) {
        cout << "No hay mapa creado. Use la opcion 1 primero." << endl;
        return false;
    }

    string ruta = "MapaAsientos/mapa_asientos.dot";
    FILE* fp = fopen(ruta.c_str(), "w");
    if (fp == nullptr) {
        fprintf(stderr, "Error al abrir el archivo '%s'\n", ruta.c_str());
        return false;
    }

    // Configuracion visual del grafo.
    fprintf(fp, "digraph Mapa_Asientos {\n");
    fprintf(fp, "rankdir=TB;\n");
    fprintf(fp, "splines=ortho;\n");
    fprintf(fp, "bgcolor=\"#f7f9fc\";\n");
    fprintf(fp, "pad=0.6;\n");
    fprintf(fp, "nodesep=0.6;\n");
    fprintf(fp, "ranksep=0.9;\n");
    fprintf(fp, "label=\"Mapa de Asientos\";\n");
    fprintf(fp, "labelloc=t;\n");
    fprintf(fp, "fontname=\"Arial\";\n");
    fprintf(fp, "node [shape=box, style=\"rounded,filled\", fontname=\"Arial\", fontsize=11, color=\"#2c3e50\"];\n");
    fprintf(fp, "edge [color=\"#b0bec5\", arrowsize=0.8, penwidth=1.2];\n");

    // El nodo central es como la raiz del grafo, de el salen las conexiones a las filas y columnas.
    fprintf(fp, "M [label=\"MAPA\", width=1.2, height=0.5, style=\"rounded,filled\", fillcolor=\"#34495e\", fontcolor=white, fontsize=13, color=\"#1b2631\", penwidth=2];\n");

    // Ponemos los encabezados de columna en el mismo nivel que el nodo central con rank=same,
    // para que visualmente queden todos alineados en la parte superior.
    fprintf(fp, "{ rank=same; M; ");
    for (int c = 1; c <= totalColumnas; c++) {
        fprintf(fp, "HC%s; ", intToString(c).c_str());
    }
    fprintf(fp, "}\n");

    // Creamos los nodos visuales de cada encabezado de columna en color naranja.
    for (int c = 1; c <= totalColumnas; c++) {
        fprintf(fp, "HC%s [label=\"C%s\", style=\"rounded,filled\", fillcolor=\"#e67e22\", fontcolor=white, fontsize=12, color=\"#d35400\", penwidth=1.5];\n",
            intToString(c).c_str(), intToString(c).c_str());
    }

    // Creamos los nodos visuales de cada encabezado de fila en color verde oscuro.
    for (int f = 1; f <= totalFilas; f++) {
        fprintf(fp, "HF%s [label=\"F%s\", style=\"rounded,filled\", fillcolor=\"#16a085\", fontcolor=white, fontsize=12, color=\"#117a65\", penwidth=1.5];\n",
            intToString(f).c_str(), intToString(f).c_str());
    }

    // Aqui es donde se crea cada nodo interno de la grilla.
    // A diferencia de la matriz dispersa tipica que solo grafica nodos con datos,
    // aca creamos un nodo por cada posicion de la grilla sin excepcion.
    // Los vacios van en gris y los ocupados en verde para que se distingan facil.
    for (int f = 1; f <= totalFilas; f++) {
        for (int c = 1; c <= totalColumnas; c++) {
            Node* nodo = findNode(intToString(f), intToString(c));
            string val = (nodo != nullptr) ? nodo->val : "---";

            if (val == "---") {
                fprintf(fp, "N%s_%s [label=\"---\", style=\"rounded,filled\", fillcolor=\"#d5dbdb\", fontcolor=\"#7f8c8d\", color=\"#aab7b8\", penwidth=1.0];\n",
                    intToString(f).c_str(), intToString(c).c_str());
            } else {
                fprintf(fp, "N%s_%s [label=\"%s\", style=\"rounded,filled\", fillcolor=\"#82e0aa\", fontcolor=\"#1b2631\", color=\"#27ae60\", penwidth=1.5];\n",
                    intToString(f).c_str(), intToString(c).c_str(), val.c_str());
            }
        }
    }

    // Con rank=same agrupamos cada fila para que visualmente queden alineadas horizontalmente.
    for (int f = 1; f <= totalFilas; f++) {
        fprintf(fp, "{ rank=same; HF%s; ", intToString(f).c_str());
        for (int c = 1; c <= totalColumnas; c++) {
            fprintf(fp, "N%s_%s; ", intToString(f).c_str(), intToString(c).c_str());
        }
        fprintf(fp, "}\n");
    }

    // Conectamos el nodo central con el primer encabezado de columna.
    if (totalColumnas > 0) {
        fprintf(fp, "M -> HC1 [color=\"#34495e\", penwidth=2, arrowhead=normal];\n");
    }

    // Conectamos los encabezados de columna entre si formando la fila superior.
    for (int c = 1; c < totalColumnas; c++) {
        fprintf(fp, "HC%s -> HC%d [color=\"#e67e22\", penwidth=2, arrowhead=normal];\n",
            intToString(c).c_str(), c + 1);
    }

    // Conectamos el nodo central con el primer encabezado de fila.
    if (totalFilas > 0) {
        fprintf(fp, "M -> HF1 [color=\"#34495e\", penwidth=2, arrowhead=normal];\n");
    }

    // Conectamos los encabezados de fila entre si formando la columna izquierda.
    for (int f = 1; f < totalFilas; f++) {
        fprintf(fp, "HF%s -> HF%d [color=\"#16a085\", penwidth=2, arrowhead=normal];\n",
            intToString(f).c_str(), f + 1);
    }

    // Ahora conectamos las celdas de cada fila entre si, de izquierda a derecha.
    // Esto es lo que forma la fila horizontal de asientos en el grafo.
    for (int f = 1; f <= totalFilas; f++) {
        fprintf(fp, "HF%s -> N%s_1 [color=\"#5dade2\", penwidth=1.5, arrowhead=normal];\n",
            intToString(f).c_str(), intToString(f).c_str());
        for (int c = 1; c < totalColumnas; c++) {
            fprintf(fp, "N%s_%d -> N%s_%d [color=\"#5dade2\", penwidth=1.5, arrowhead=normal];\n",
                intToString(f).c_str(), c, intToString(f).c_str(), c + 1);
        }
    }

    // Conectamos las celdas de cada columna entre si, de arriba hacia abajo.
    // Esto es lo que forma la columna vertical de asientos en el grafo.
    for (int c = 1; c <= totalColumnas; c++) {
        fprintf(fp, "HC%s -> N1_%s [color=\"#eb984e\", penwidth=1.5, arrowhead=normal];\n",
            intToString(c).c_str(), intToString(c).c_str());
        for (int f = 1; f < totalFilas; f++) {
            fprintf(fp, "N%d_%s -> N%d_%s [color=\"#eb984e\", penwidth=1.5, arrowhead=normal];\n",
                f, intToString(c).c_str(), f + 1, intToString(c).c_str());
        }
    }

    fprintf(fp, "}\n");
    fclose(fp);

    // Compilamos el archivo .dot a una imagen PNG usando Graphviz.
#ifdef _WIN32
    int dotResult = system("dot -Tpng MapaAsientos/mapa_asientos.dot -o MapaAsientos/mapa_asientos.png");
#else
    int dotResult = system("dot -Tpng MapaAsientos/mapa_asientos.dot -o MapaAsientos/mapa_asientos.png 2>/dev/null");
#endif

    if (dotResult != 0) {
        cout << "Se genero el archivo mapa_asientos.dot." << endl;
        cout << "No se pudo generar el PNG (Graphviz no disponible)." << endl;
        cout << "Instalalo y ejecuta: dot -Tpng mapa_asientos.dot -o mapa_asientos.png" << endl;
        return true;
    }

    // En Windows abrimos la imagen automaticamente con el visor predeterminado.
#ifdef _WIN32
    system("start MapaAsientos/mapa_asientos.png");
#else
    cout << "Imagen generada en MapaAsientos/mapa_asientos.png" << endl;
#endif

    return true;
}

// El menu principal le da al usuario las opciones para interactuar con el mapa de asientos.
// Se repite hasta que el usuario elija salir.
int main() {
    MapaAsientos mapa;
    int opcion;

    do {
        cout << endl;
        cout << "========== M E N U ===========" << endl;
        cout << "1. Crear mapa de asientos" << endl;
        cout << "2. Ver mapa de asientos" << endl;
        cout << "3. Graficar matriz" << endl;
        cout << "4. Salir" << endl;
        cout << "Ingrese una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        // Esta opcion pide las dimensiones del mapa y construye la grilla completa.
        case 1: {
            int filas, columnas;
            cout << "Ingrese el numero de filas: ";
            cin >> filas;
            cout << "Ingrese el numero de columnas: ";
            cin >> columnas;
            cin.ignore();

            if (filas <= 0 || columnas <= 0) {
                cout << "Las dimensiones deben ser mayores a 0." << endl;
            } else {
                mapa.crearMapa(filas, columnas);
            }
            break;
        }
        // Muestra el mapa en consola con una tabla formateada.
        case 2:
            mapa.verMapa();
            break;
        // Genera la imagen del mapa con Graphviz.
        case 3:
            mapa.graficar();
            break;
        case 4:
            cout << "Saliendo del programa..." << endl;
            break;
        default:
            cout << "Opcion no valida." << endl;
            break;
        }
    } while (opcion != 4);

    return 0;
}
