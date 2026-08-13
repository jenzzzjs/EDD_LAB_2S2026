# Árbol Binario de Búsqueda de Películas

## ¿Qué es un árbol binario de búsqueda?

Es una estructura jerárquica donde cada nodo tiene a lo sumo **dos hijos** (izquierdo y derecho). La regla del árbol binario de búsqueda es: el hijo izquierdo contiene valores **menores** que el nodo y el hijo derecho valores **mayores**. Esto permite buscar, insertar y eliminar de forma muy eficiente.

En esta semana el árbol guarda a las **películas** de cine. Cada película tiene 2 campos fundamentales:

- **Código de película**
- **Nombre de la película**

El árbol se ordena por el **código de película** de forma **numérica** (se extrae el número del código, por ejemplo `P004` = 4 y `P0018` = 18):

- **Nodo izquierdo**: película con código numérico menor.
- **Nodo derecho**: película con código numérico mayor.

```
             (Titanic - P0044)
             /                \
     (Avatar - P004)      (Parasitos - P035)
```

## Estructura del proyecto

| Archivo               | Contenido                                                            |
|-----------------------|----------------------------------------------------------------------|
| `ArbolBstPeliculas.cpp` | Clase `Pelicula`, clase `NodoArbolPeliculas`, clase `ArbolBinarioDeBusquedaPeliculas` y `main()` |
| `peliculas.csv`        | Ejemplo de datos con formato `codigo,nombre` listo para probar       |

## Como funciona

1. **`insertar(pelicula)`**: inserta una película en el árbol respetando el orden por código. Si el código ya existe, no duplica el nodo.

2. **`cargarCSV(ruta)`**: lee un archivo CSV con formato `codigo,nombre`. La primera línea se ignora (encabezado) y el resto de películas se insertan en el árbol.

3. **`generarDot()`**: genera el archivo `arbol_peliculas.dot` con el código DOT del árbol y lo convierte a `arbol_peliculas.png` usando Graphviz, luego abre la imagen.

## Compilación y ejecución

### Compilar desde Semana_6

```bash
cd Semana_6
g++ ArbolBstPeliculas.cpp -o ArbolBstPeliculas
./ArbolBstPeliculas
```

### Compilar desde la raíz del repositorio

```bash
g++ Semana_6/ArbolBstPeliculas.cpp -o Semana_6/ArbolBstPeliculas
./Semana_6/ArbolBstPeliculas
```

> **Nota:** En Windows el ejecutable se genera como `ArbolBstPeliculas.exe`. Ejecutar desde la carpeta `Semana_6/` para que los archivos `arbol_peliculas.dot` y `arbol_peliculas.png` se generen en el lugar correcto.

## Reporte gráfico

La opción **2** del menú genera automáticamente el código DOT y la imagen PNG con Graphviz:

```bash
dot -Tpng arbol_peliculas.dot -o arbol_peliculas.png
```

Requisito: tener instalado [Graphviz](https://graphviz.org/).

## Menú del sistema

1. Insertar película
2. Graficar árbol
3. Cargar CSV con películas
0. Salir

- **1. Insertar película**: pide código y nombre, e inserta la película en el árbol.
- **2. Graficar árbol**: genera `arbol_peliculas.dot` y `arbol_peliculas.png`.
- **3. Cargar CSV con películas**: pide la ruta de un archivo CSV (`codigo,nombre`) e inserta todas las películas. Incluye un ejemplo listo para probar: `peliculas.csv`.
- **0. Salir**: termina el programa.

## Cargar películas desde CSV

La opción **3** permite cargar un archivo CSV con el formato `codigo,nombre`. Ejemplo:

```csv
codigo,nombre
P001,El Padrino
P002,Titanic
P003,Inception
```

El proyecto incluye un ejemplo listo para probar: `peliculas.csv`. Si ejecutas desde `Semana_6/`, la ruta es `peliculas.csv`.
