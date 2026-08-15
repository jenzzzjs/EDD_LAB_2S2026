// spotify.cpp - menu principal del sistema de gestion de musica
#include <iostream>   // Para mostrar mensajes y leer lo que escribe el usuario
#include <fstream>    // Para leer y escribir archivos como el CSV y el DOT
#include <string>     // Para usar el tipo string
#include <cstdlib>    // Para convertir texto a numero y ejecutar Graphviz
#include <cctype>     // Para verificar que un caracter sea digito
#include "Artistas.h" // Lista de listas, artistas con sus colas de canciones
#include "Canciones.h"
using namespace std;

// Instancia global para que todas las funciones del menu la usen
ListaArtistas listaArtistas;

void mostrarMenu() {
    cout << "\n" << string(50, '=') << "\n";
    cout << "    SISTEMA DE GESTION DE MUSICA\n";
    cout << string(50, '=') << "\n";
    cout << "1. Ingresar un artista\n";
    cout << "2. Ingresar una cancion\n";
    cout << "3. Cargar archivo CSV\n";
    cout << "4. Eliminar una cancion\n";
    cout << "5. Generar reporte grafico\n";
    cout << "6. Salir\n";
    cout << string(50, '=') << "\n";
    cout << "Seleccione una opcion: ";
}

// Espera el Enter del usuario, con getline para no dejar basura en el buffer
void pausar() {
    string enter;
    getline(cin, enter);
}

// Quita los espacios y saltos de linea del inicio y final de una cadena
string trim(string texto) {
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == string::npos) {
        return "";
    }
    size_t fin = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fin - inicio + 1);
}

void ingresarArtista() {
    cout << "\n" << string(50, '-') << "\n";
    cout << "   INGRESAR NUEVO ARTISTA\n";
    cout << string(50, '-') << "\n";

    cout << "Nombre del artista: ";
    string nombre;
    getline(cin, nombre); // getline permite nombres con espacios

    // El nombre no puede estar vacio
    if (nombre.empty()) {
        cout << "\nError: El nombre no puede estar vacio.\n";
        cout << "Presione Enter para continuar...";
        pausar();
        return;
    }

    // No puede haber dos artistas con el mismo nombre
    if (listaArtistas.existe(nombre)) {
        cout << "\nError: El artista '" << nombre << "' ya existe en el sistema.\n";
    } else {
        // Se inserta el nuevo artista al final de la lista doble
        listaArtistas.insertar(nombre);
        cout << "\nArtista '" << nombre << "' agregado exitosamente!\n";
    }

    cout << "Presione Enter para continuar...";
    pausar();
}

void ingresarCancion() {
    cout << "\n" << string(50, '-') << "\n";
    cout << "   INGRESAR NUEVA CANCION\n";
    cout << string(50, '-') << "\n";

    cout << "Nombre del artista: ";
    string artista;
    getline(cin, artista);

    // El artista debe existir en la lista
    if (!listaArtistas.existe(artista)) {
        cout << "\nError: El artista '" << artista << "' no existe en el sistema.\n";
        cout << "Primero debe agregar el artista en la opcion 1.\n";
        cout << "Presione Enter para continuar...";
        pausar();
        return;
    }

    cout << "Nombre de la cancion: ";
    string cancion;
    getline(cin, cancion);

    // El nombre de la cancion no puede estar vacio
    if (cancion.empty()) {
        cout << "\nError: El nombre de la cancion no puede estar vacio.\n";
        cout << "Presione Enter para continuar...";
        pausar();
        return;
    }

    cout << "Anio de lanzamiento: ";
    string anioStr;
    getline(cin, anioStr);

    // El anio se lee como texto y se valida que tenga 4 digitos,
    // asi no se rompe el buffer si el usuario escribe letras
    bool anioValido = true;
    if (anioStr.length() != 4) {
        anioValido = false;
    } else {
        // Se recorre cada caracter y se verifica que sea digito
        for (size_t i = 0; i < anioStr.length(); i++) {
            if (!isdigit(anioStr[i])) {
                anioValido = false;
                break;
            }
        }
    }

    int anio = 0;
    if (anioValido) {
        // Se convierte el texto a numero y se valida el rango
        anio = atoi(anioStr.c_str());
        if (anio < 1900 || anio > 2050) {
            anioValido = false;
        }
    }

    if (!anioValido) {
        cout << "\nError: Anio invalido. Debe ser un numero de 4 digitos entre 1900 y 2050.\n";
        cout << "Presione Enter para continuar...";
        pausar();
        return;
    }

    // Se encola la cancion en la cola del artista
    if (listaArtistas.agregarCancionAArtista(artista, cancion, anio)) {
        cout << "\nCancion '" << cancion << "' agregada exitosamente al artista '" << artista << "'!\n";
    } else {
        cout << "\nError: No se pudo agregar la cancion.\n";
    }

    cout << "Presione Enter para continuar...";
    pausar();
}

void cargarCSV() {
    cout << "\n" << string(50, '-') << "\n";
    cout << "   CARGAR ARCHIVO CSV\n";
    cout << string(50, '-') << "\n";

    cout << "Nombre del archivo CSV (formato: artista,cancion,anio): ";
    string nombreArchivo;
    getline(cin, nombreArchivo);

    // Se intenta abrir el archivo en modo lectura
    ifstream archivo(nombreArchivo.c_str());
    if (!archivo.is_open()) {
        cout << "\nError: No se pudo abrir el archivo '" << nombreArchivo << "'.\n";
        cout << "Presione Enter para continuar...";
        pausar();
        return;
    }

    string linea;
    int cargados = 0;
    bool primeraLinea = true; // Para saber si el archivo trae cabecera

    while (getline(archivo, linea)) {
        linea = trim(linea);   // Se quitan los espacios de mas
        if (linea.empty()) {
            continue;          // Se saltan las lineas en blanco
        }

        // Si la primera linea trae la palabra artista es la cabecera
        if (primeraLinea) {
            primeraLinea = false;
            if (linea.find("artista") != string::npos) {
                continue;
            }
        }

        // Se buscan las dos comas que separan los 3 campos
        size_t p1 = linea.find(',');
        if (p1 == string::npos) {
            continue; // Sin coma no es un registro valido
        }
        size_t p2 = linea.find(',', p1 + 1);
        if (p2 == string::npos) {
            continue; // Sin las dos comas no estan los 3 campos
        }

        // Se recortan los 3 campos y se les quitan los espacios
        string artista = trim(linea.substr(0, p1));
        string cancion = trim(linea.substr(p1 + 1, p2 - p1 - 1));
        string anioStr = trim(linea.substr(p2 + 1));

        // Los campos deben venir llenos y el anio tener 4 caracteres
        if (artista.empty() || cancion.empty() || anioStr.length() != 4) {
            continue;
        }

        // Todos los caracteres del anio deben ser digitos
        bool anioValido = true;
        for (size_t i = 0; i < anioStr.length(); i++) {
            if (!isdigit(anioStr[i])) {
                anioValido = false;
                break;
            }
        }

        if (!anioValido) {
            continue;
        }

        int anio = atoi(anioStr.c_str());
        if (anio < 1900 || anio > 2050) {
            continue;
        }

        // Si el artista no existe se crea primero
        if (!listaArtistas.existe(artista)) {
            listaArtistas.insertar(artista);
        }

        // Se encola la cancion en la cola del artista
        listaArtistas.agregarCancionAArtista(artista, cancion, anio);
        cargados++;
    }

    archivo.close();

    // Resumen de lo que se cargo
    if (cargados > 0) {
        cout << "\nSe cargaron " << cargados << " canciones desde el archivo.\n";
        cout << "  - Artistas registrados: " << listaArtistas.obtenerTamanio() << "\n";
        cout << "  - Canciones totales: " << listaArtistas.contarCancionesTotales() << "\n";
    } else {
        cout << "\nNo se encontro ningun registro valido en el archivo.\n";
    }

    cout << "Presione Enter para continuar...";
    pausar();
}

void eliminarCancion() {
    cout << "\n" << string(50, '-') << "\n";
    cout << "   ELIMINAR UNA CANCION\n";
    cout << string(50, '-') << "\n";

    cout << "Nombre del artista: ";
    string artista;
    getline(cin, artista);

    // El artista debe existir en el sistema
    if (!listaArtistas.existe(artista)) {
        cout << "\nError: El artista '" << artista << "' no existe en el sistema.\n";
        cout << "Presione Enter para continuar...";
        pausar();
        return;
    }

    // Se obtienen las canciones del artista para mostrarlas
    ArregloDinamico<ParCancion> canciones = listaArtistas.obtenerCancionesDeArtista(artista);
    if (canciones.empty()) {
        cout << "\nEl artista '" << artista << "' no tiene canciones registradas.\n";
        cout << "Presione Enter para continuar...";
        pausar();
        return;
    }

    cout << "\nCanciones de '" << artista << "' (" << canciones.size() << "):\n";
    cout << string(30, '-') << "\n";
    for (int c = 0; c < canciones.size(); c++) {
        cout << "  * " << canciones[c].nombre << " (" << canciones[c].anio << ")\n";
    }

    // Por FIFO se eliminara la primera cancion que entro a la cola
    cout << "\nLas canciones se guardan en una COLA (FIFO), por lo que se eliminara ";
    cout << "la primera en entrar:\n";
    cout << "  > " << canciones[0].nombre << " (" << canciones[0].anio << ")\n";

    // Se desencola y los datos de la eliminada se devuelven por referencia
    string nombre;
    int anio;
    if (listaArtistas.eliminarPrimeraCancionDeArtista(artista, nombre, anio)) {
        cout << "\nCancion '" << nombre << "' eliminada exitosamente de '" << artista << "'!\n";
    } else {
        cout << "\nError: No se pudo eliminar la cancion.\n";
    }

    cout << "Presione Enter para continuar...";
    pausar();
}

void generarReporte() {
    cout << "\n" << string(50, '-') << "\n";
    cout << "   GENERAR REPORTE GRAFICO\n";
    cout << string(50, '-') << "\n";

    // Sin artistas no hay nada que graficar
    if (listaArtistas.obtenerTamanio() == 0) {
        cout << "\nError: No hay datos para generar el reporte.\n";
        cout << "Agregue artistas primero.\n";
        cout << "Presione Enter para continuar...";
        pausar();
        return;
    }

    // Nombre fijo para los archivos de salida
    string dotFile = "reporte_musica.dot";
    string pngFile = "reporte_musica.png";

    // Se escribe el codigo DOT que genera la lista en un archivo
    ofstream archivo(dotFile.c_str());
    if (!archivo.is_open()) {
        cout << "\nError: No se pudo crear el archivo DOT.\n";
        cout << "Presione Enter para continuar...";
        pausar();
        return;
    }
    archivo << listaArtistas.generarDotCompleto();
    archivo.close();

    cout << "\nArchivo DOT generado: " << dotFile << "\n";
    cout << "\nContenido del sistema:\n";
    cout << string(30, '-') << "\n";

    // Resumen de lo que se va a graficar
    ArregloDinamico<NodoArtista*> artistas = listaArtistas.obtenerArtistas();
    for (int i = 0; i < artistas.size(); i++) {
        cout << "* " << artistas[i]->nombre << " (" << artistas[i]->cantidadCanciones() << " canciones)\n";
    }

    cout << "\nGenerando imagen PNG...\n";

    // system lanza Graphviz para convertir el DOT en PNG
    string comando = "dot -Tpng " + dotFile + " -o " + pngFile;
    system(comando.c_str());

    // Se verifica que el PNG se haya creado
    ifstream check(pngFile.c_str());
    if (check.good()) {
        check.close();
        cout << "\nImagen generada exitosamente: " << pngFile << "\n";
        cout << "  - Artistas registrados: " << listaArtistas.obtenerTamanio() << "\n";
        cout << "  - Canciones totales: " << listaArtistas.contarCancionesTotales() << "\n";
    } else {
        cout << "\nError al generar la imagen.\n";
        cout << "Intente ejecutar manualmente:\n";
        cout << "  dot -Tpng " << dotFile << " -o " << pngFile << "\n";
    }
}

// Mensaje de despedida cuando el usuario elige salir
void mostrarCreditos() {
    cout << "\n" << string(50, '=') << "\n";
    cout << "    Gracias por usar el sistema\n";
    cout << "    Lista de Listas - Semana 4\n";
    cout << string(50, '=') << "\n\n";
}

int main() {
    int opcion = 0;

    while (true) {
        mostrarMenu();

        // Si la lectura del numero falla, la entrada llego a su fin
        if (!(cin >> opcion)) {
            break;
        }
        pausar(); // Se consume el Enter que deja la lectura del numero

        // Ruta segun la opcion elegida
        if (opcion == 1) {
            ingresarArtista();
        } else if (opcion == 2) {
            ingresarCancion();
        } else if (opcion == 3) {
            cargarCSV();
        } else if (opcion == 4) {
            eliminarCancion();
        } else if (opcion == 5) {
            generarReporte();
        } else if (opcion == 6) {
            mostrarCreditos();
            break;
        } else {
            cout << "\nOpcion invalida. Por favor, seleccione 1-6.\n";
            cout << "Presione Enter para continuar...";
            pausar();
        }
    }

    return 0;
}
