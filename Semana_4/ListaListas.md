# Lista de Listas

## ¿Qué es una lista de listas?

Es una estructura de datos compuesta en la que **cada nodo de una lista principal contiene otra estructura de datos completa** (como otra lista, una cola o una pila). Es decir, una lista que en lugar de almacenar datos simples, almacena estructuras anidadas.

### Ejemplo: Sistema de Música (Spotify)

En el ejemplo de esta semana, la estructura es:

- **Lista doblemente enlazada de artistas**: cada `NodoArtista` tiene punteros `siguiente` y `anterior`, y guarda un nombre.
- **Cola de canciones por artista**: cada `NodoArtista` contiene su propia `ColaCanciones` (estructura FIFO) con las canciones de ese artista.

```
ListaArtistas (doble)
    │
    ├── NodoArtista "Shakira"  ──► ColaCanciones: Hips Don't Lie (2006) → Waka Waka (2010)
    ├── NodoArtista "Rihanna"  ──► ColaCanciones: Umbrella (2007)
    └── NodoArtista "Coldplay" ──► ColaCanciones: Yellow (2000) → Viva la Vida (2008)
```

### Ventajas

- Permite organizar y acceder a datos con jerarquía (artista → canciones).
- Cada subestructura puede ser gestionada con su propia lógica (en este caso, cola FIFO para canciones).
- Es útil para representar catálogos, menús, matrices dispersas y jerarquías en general.

## Estructura del proyecto

El proyecto está organizado en 2 archivos de estructuras más el menú principal:

| Archivo       | Contenido                                                      |
|---------------|----------------------------------------------------------------|
| `Canciones.h` | Nodo de canción (`NodoCancion`) y cola FIFO (`ColaCanciones`)  |
| `Artistas.h`  | Nodo de artista (`NodoArtista`) y lista doble (`ListaArtistas`)|
| `spotify.cpp` | Menú principal y lógica de la interfaz                         |

## Compilación y ejecución

### Compilar desde la carpeta del proyecto

```bash
cd Semana_4/Spotify
g++ spotify.cpp -o spotify
./spotify
```

### Compilar desde la raíz del repositorio

```bash
g++ Semana_4/Spotify/spotify.cpp -o Semana_4/Spotify/spotify
./Semana_4/Spotify/spotify
```

> **Nota:** En Windows el ejecutable se genera como `spotify.exe`. Ejecutar desde la carpeta `Semana_4/Spotify/` para que los archivos `reporte_musica.dot` y `reporte_musica.png` se generen en el lugar correcto.

## Reporte gráfico

La opción **5** del menú genera automáticamente el código DOT y la imagen PNG con Graphviz, sin pedir confirmación ni esperar Enter:

```bash
dot -Tpng reporte_musica.dot -o reporte_musica.png
```

Requisito: tener instalado [Graphviz](https://graphviz.org/).

## Cargar canciones desde CSV

La opción **3** permite cargar un archivo CSV con el formato `artista,cancion,anio`. Ejemplo:

```csv
artista,cancion,anio
Shakira,Hips Don't Lie,2006
Rihanna,Umbrella,2007
```

El proyecto incluye un ejemplo listo para probar: `ejemplo_canciones.csv`.

## Eliminar una canción

La opción **4** elimina una canción de un artista. Como las canciones de cada artista se guardan en una **cola (FIFO)**, se elimina siempre la **primera canción que entró** (la que está al frente de la fila).

## Menú del sistema

1. Ingresar un artista
2. Ingresar una canción
3. Cargar archivo CSV
4. Eliminar una canción
5. Generar reporte gráfico
6. Salir
