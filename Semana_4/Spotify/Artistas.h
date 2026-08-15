// Artistas.h - lista doble de artistas, cada uno con su cola de canciones
#ifndef ARTISTAS_H
#define ARTISTAS_H

#include <string>   // Para usar el tipo string
#include <cstdio>   // Para snprintf que arma el color hexadecimal del DOT
#include "Canciones.h" // Cada artista tiene su propia cola de canciones
using namespace std;

class NodoArtista {
public:
    string nombre;               // Nombre del artista
    ColaCanciones colaCanciones; // Cola FIFO donde guarda sus canciones
    NodoArtista* siguiente;      // Puntero al siguiente artista
    NodoArtista* anterior;       // Puntero al artista anterior

    // El artista inicia sin conexiones y con su cola vacia
    NodoArtista(string nombre) {
        this->nombre = nombre;
        this->siguiente = nullptr;
        this->anterior = nullptr;
    }

    // Llama a la cola interna del artista para encolar la cancion
    bool agregarCancion(string nombre, int anio) {
        return colaCanciones.encolar(nombre, anio);
    }

    // Elimina la primera cancion de la cola del artista (FIFO)
    bool eliminarPrimeraCancion(string& nombre, int& anio) {
        return colaCanciones.desencolar(nombre, anio);
    }

    // Devuelve todas las canciones del artista en un arreglo dinámico
    ArregloDinamico<ParCancion> obtenerCanciones() {
        return colaCanciones.obtenerTodasCanciones();
    }

    // Cuantas canciones tiene el artista en su cola
    int cantidadCanciones() {
        return colaCanciones.obtenerTamanio();
    }
};

class ListaArtistas {
private:
    NodoArtista* cabeza; // Puntero al primer artista
    NodoArtista* cola;   // Puntero al ultimo artista
    int tamanio;         // Cantidad de artistas

public:
    // La lista inicia vacia con los punteros en nullptr
    ListaArtistas() {
        cabeza = nullptr;
        cola = nullptr;
        tamanio = 0;
    }

    // Destructor: libera los artistas y sus colas en cascada
    ~ListaArtistas() {
        NodoArtista* actual = cabeza;
        while (actual != nullptr) {
            NodoArtista* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
    }

    // Inserta un artista al final, la cola guardada evita recorrer la lista
    bool insertar(string nombre) {
        NodoArtista* nuevo = new NodoArtista(nombre);

        if (cabeza == nullptr) {
            // Lista vacia, el nuevo nodo es el unico
            cabeza = nuevo;
            cola = nuevo;
        } else {
            // Se enlaza despues de la cola actual
            nuevo->anterior = cola;   // El nuevo apunta hacia atras
            cola->siguiente = nuevo;  // La cola vieja apunta al nuevo
            cola = nuevo;             // El nuevo ahora es la cola
        }

        tamanio++;
        return true;
    }

    // Busca un artista por nombre, devuelve nullptr si no existe
    NodoArtista* buscar(string nombre) {
        NodoArtista* actual = cabeza;

        while (actual != nullptr) {
            if (actual->nombre == nombre) {
                return actual;
            }
            actual = actual->siguiente;
        }

        return nullptr;
    }

    // Indica si un artista existe, retorna true cuando buscar lo encuentra
    bool existe(string nombre) {
        return buscar(nombre) != nullptr;
    }

    // Devuelve todos los artistas en un arreglo dinámico para mostrarlos
    ArregloDinamico<NodoArtista*> obtenerArtistas() {
        ArregloDinamico<NodoArtista*> artistas;
        NodoArtista* actual = cabeza;

        while (actual != nullptr) {
            artistas.agregar(actual);
            actual = actual->siguiente;
        }

        return artistas;
    }

    // Agrega una cancion al artista indicado (false si no existe)
    bool agregarCancionAArtista(string nombreArtista, string nombreCancion, int anio) {
        NodoArtista* artista = buscar(nombreArtista);
        if (artista == nullptr) {
            return false;
        }

        return artista->agregarCancion(nombreCancion, anio);
    }

    // Devuelve las canciones del artista indicado (vacio si no existe)
    ArregloDinamico<ParCancion> obtenerCancionesDeArtista(string nombreArtista) {
        NodoArtista* artista = buscar(nombreArtista);
        if (artista == nullptr) {
            return ArregloDinamico<ParCancion>();
        }

        return artista->obtenerCanciones();
    }

    // Desencola la primera cancion del artista (false si no existe)
    bool eliminarPrimeraCancionDeArtista(string nombreArtista, string& nombre, int& anio) {
        NodoArtista* artista = buscar(nombreArtista);
        if (artista == nullptr) {
            return false;
        }

        return artista->eliminarPrimeraCancion(nombre, anio);
    }

    // Cuantos artistas hay en la lista
    int obtenerTamanio() {
        return tamanio;
    }

    // Suma las canciones de todos los artistas
    int contarCancionesTotales() {
        int total = 0;
        NodoArtista* actual = cabeza;

        while (actual != nullptr) {
            total += actual->cantidadCanciones();
            actual = actual->siguiente;
        }

        return total;
    }

    // Construye el codigo DOT de Graphviz para dibujar toda la estructura
    string generarDotCompleto() {
        string dot = "";
        // Cabecera del grafo, la direccion va de arriba hacia abajo
        dot += "digraph SistemaMusica {\n";
        dot += "    rankdir=TB;\n";
        dot += "    node [fontname=\"Helvetica\", fontsize=12];\n";
        dot += "    edge [fontname=\"Helvetica\", fontsize=10];\n";
        dot += "    compound=true;\n";
        dot += "    bgcolor=\"white\";\n";
        dot += "    ranksep=0.5;\n";
        dot += "    nodesep=0.4;\n\n";
        dot += "    graph [splines=ortho];\n";
        dot += "    edge [arrowsize=0.8, color=black, fontcolor=black];\n\n";

        // Se guardan los artistas y cuantas canciones tiene cada uno
        ArregloDinamico<NodoArtista*> artistas = obtenerArtistas();
        int totalArtistas = artistas.size();
        ArregloDinamico<int> cancionesPorArtista;
        for (int i = 0; i < totalArtistas; i++) {
            cancionesPorArtista.agregar(0);
        }

        // 1) Nodos de los artistas, todos en un mismo rango para la fila superior
        dot += "    {\n";
        dot += "        rank=same;\n";
        for (int i = 0; i < totalArtistas; i++) {
            dot += "        artista" + to_string(i) + " [label=\"" + artistas[i]->nombre +
                   "\", shape=box, style=\"filled,rounded\", fillcolor=\"black\", " +
                   "color=\"black\", penwidth=2, width=1.8, height=0.8, " +
                   "fontname=\"Helvetica-Bold\", fontcolor=\"white\"];\n";
        }
        dot += "    }\n\n";

        // 2) Flechas siguiente y anterior entre artistas consecutivos
        dot += "    // CONEXIONES ENTRE ARTISTAS\n";
        for (int i = 0; i < totalArtistas; i++) {
            if (i < totalArtistas - 1) {
                dot += "    artista" + to_string(i) + " -> artista" + to_string(i + 1) +
                       " [color=black, label=\"siguiente\", dir=forward, " +
                       "penwidth=1.5, arrowhead=vee];\n";
                dot += "    artista" + to_string(i + 1) + " -> artista" + to_string(i) +
                       " [color=black, label=\"anterior\", constraint=false, " +
                       "penwidth=1.5, arrowhead=vee];\n";
            }
        }

        dot += "\n";

        // 3) Un nodo por cancion, se guarda el conteo por artista
        for (int a = 0; a < totalArtistas; a++) {
            ArregloDinamico<ParCancion> canciones = artistas[a]->obtenerCanciones();
            int contadorC = canciones.size();
            cancionesPorArtista[a] = contadorC;

            dot += "    // CANCIONES DE: " + artistas[a]->nombre + "\n";
            if (contadorC > 0) {
                for (int c = 0; c < contadorC; c++) {
                    // Color azul fijo, snprintf arma el hexadecimal
                    int r = 30;
                    int g = 60;
                    int b = 160;
                    char hex[8];
                    snprintf(hex, sizeof(hex), "#%02x%02x%02x", r, g, b);
                    string colorFill = hex;

                    // Cada cancion es un nodo con su nombre y anio
                    dot += "    cancion" + to_string(a) + "_" + to_string(c) + " [label=\"" +
                           canciones[c].nombre + "\\n(" + to_string(canciones[c].anio) + ")\", " +
                           "shape=box, style=\"filled\", fillcolor=\"" + colorFill + "\", " +
                           "color=\"black\", penwidth=1.5, width=1.4, height=0.9, " +
                           "fontcolor=\"white\", fontname=\"Helvetica\"];\n";
                }
            } else {
                // Artista sin canciones, etiqueta gris
                dot += "    sin_canciones" + to_string(a) + " [label=\"SIN CANCIONES\", " +
                       "shape=plaintext, fontcolor=\"gray\", fontsize=9, " +
                       "fontname=\"Helvetica-Italic\"];\n";
            }

            dot += "\n";
        }

        // 4) Flecha del artista hacia su primera cancion
        dot += "    // CONEXION ARTISTA -> PRIMERA CANCION\n";
        for (int a = 0; a < totalArtistas; a++) {
            if (cancionesPorArtista[a] > 0) {
                dot += "    artista" + to_string(a) + " -> cancion" + to_string(a) + "_0 " +
                       "[color=black, dir=forward, penwidth=2, arrowhead=vee];\n";
            } else {
                dot += "    artista" + to_string(a) + " -> sin_canciones" + to_string(a) +
                       " [style=dotted, color=\"gray\", minlen=1, penwidth=1];\n";
            }
        }

        dot += "\n";

        // 5) Cadena de canciones de cada artista, el peso mantiene la columna unida
        dot += "    // CADENA DE CANCIONES DE CADA ARTISTA\n";
        for (int a = 0; a < totalArtistas; a++) {
            for (int c = 0; c < cancionesPorArtista[a] - 1; c++) {
                dot += "    cancion" + to_string(a) + "_" + to_string(c) + " -> cancion" +
                       to_string(a) + "_" + to_string(c + 1) +
                       " [color=black, dir=forward, penwidth=1.5, arrowhead=vee, weight=10];\n";
            }
        }

        dot += "\n";

        // 6) Rank=same por fila para que las columnas de canciones queden rectas
        int maxCanciones = 0;
        for (int a = 0; a < totalArtistas; a++) {
            if (cancionesPorArtista[a] > maxCanciones) {
                maxCanciones = cancionesPorArtista[a];
            }
        }

        dot += "    // FILAS DE CANCIONES (alineacion vertical)\n";
        for (int fila = 0; fila < maxCanciones; fila++) {
            dot += "    { rank=same;";
            for (int a = 0; a < totalArtistas; a++) {
                if (fila < cancionesPorArtista[a]) {
                    dot += " cancion" + to_string(a) + "_" + to_string(fila);
                }
            }
            dot += " }\n";
        }

        dot += "\n";

        // Nodo invisible de cierre para equilibrar el grafo
        dot += "    titulo_invisible [label=\"\", shape=plaintext, width=0, height=0];\n";
        dot += "}\n";

        return dot;
    }
};

#endif
