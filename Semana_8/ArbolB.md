# Árbol B

## ¿Qué es un árbol B?

Es una estructura jerárquica **auto-balanceada** donde cada nodo puede tener **más de dos hijos**. A diferencia del árbol binario, aquí cada nodo almacena varias claves ordenadas y varios punteros a hijos, lo que reduce la altura del árbol y hace que las búsquedas sean más rápidas.

El árbol B se define por un parámetro **m** (el orden del árbol):

- Cada nodo tiene a lo máximo **m hijos** y **m - 1 claves**.
- Cuando un nodo se llena al insertar, se **divide** y su clave mediana **sube al padre**.
- Todas las hojas están al **mismo nivel**, por eso el árbol siempre queda balanceado.

En esta semana el árbol guarda las **canciones** de un sistema musical. Cada canción tiene 3 campos:

- **Código**
- **Nombre**
- **Artista**

El árbol se ordena por el **código**:

- Las claves dentro de cada nodo están ordenadas de menor a mayor.
- El puntero a la izquierda de una clave lleva a códigos **menores**, el puntero a la derecha a códigos **mayores**.

```
                 [ C03 ]
                /      \
        [C01 | C02]  [C04 | C05]
```

## Estructura del proyecto

| Archivo          | Contenido                                                                                     |
|------------------|-----------------------------------------------------------------------------------------------|
| `ArbolB.cpp`     | Clase `Cancion`, clase `NodoArbolB`, clase `ArbolBCanciones`, lector de JSON y `main()`       |
| `canciones.json` | Archivo de ejemplo con 10 canciones para cargar al árbol                                       |
| `canciones.csv`  | Archivo de ejemplo con 15 canciones para cargar al árbol                                       |
| `arbol_b.dot`    | Código DOT que genera el programa                                                              |
| `arbol_b_canciones.png` | Imagen del árbol generada con Graphviz                                                  |

## Como funciona

1. **`insertar(cancion)`**: inserta una canción en el árbol respetando el orden por código. Si el código ya existe, no duplica el nodo. Al insertar en un nodo lleno, el nodo se divide y la clave mediana sube al padre (puede crecer la raíz y por lo tanto la altura del árbol).

2. **`buscar(codigo)`**: busca una canción por su código recorriendo el árbol desde la raíz. Dentro de cada nodo avanza por las claves hasta encontrar el código o bajar por el puntero correspondiente.

3. **`cargarJSON(ruta)`**: lee un archivo JSON (arreglo de objetos con `codigo`, `nombre` y `artista`) e inserta todas las canciones en el árbol. Ignora duplicados e ignora objetos sin código.

4. **`cargarCSV(ruta)`**: lee un archivo CSV con formato `codigo,nombre,artista`. La primera línea se ignora (encabezado) y el resto de canciones se insertan en el árbol. Si el artista contiene comas, se puede encerrar entre comillas dobles. Ignora duplicados.

5. **`eliminar(codigo)`**: elimina una canción del árbol por su código y rebalancea el árbol para que siga siendo un árbol B válido:

   - **Eliminar en hoja**: quita la clave directamente.
   - **Eliminar en nodo interno**: reemplaza la clave por su **predecesor** (máximo del subárbol izquierdo) o su **sucesor** (mínimo del subárbol derecho), y luego elimina ese reemplazo de la hoja.
   - **Rebalanceo**: si un nodo queda con menos claves del mínimo (`(m+1)/2 - 1`), primero intenta **tomar prestado** de un hermano (rotación a través del padre); si ningún hermano puede prestar, **fusiona** con un hermano y la clave separadora baja del padre. Si el padre queda bajo el mínimo, el proceso se repite hacia arriba.
   - **Raíz**: si la raíz queda sin claves, su único hijo pasa a ser la nueva raíz y la altura del árbol disminuye.

6. **`graficar()`**: genera el archivo `arbol_b.dot` con el código DOT del árbol y lo convierte a `arbol_b_canciones.png` usando Graphviz, luego abre la imagen.

## Compilación y ejecución

### Compilar desde Semana_8

```bash
cd Semana_8
g++ ArbolB/ArbolB.cpp -o ArbolB/ArbolB.exe
./ArbolB/ArbolB.exe
```

### Compilar desde la raíz del repositorio

```bash
g++ Semana_8/ArbolB/ArbolB.cpp -o Semana_8/ArbolB/ArbolB.exe
./Semana_8/ArbolB/ArbolB.exe
```

> **Nota:** Ejecutar desde la carpeta `Semana_8/ArbolB/` para que los archivos `arbol_b.dot` y `arbol_b_canciones.png` se generen en el lugar correcto.

## Reporte gráfico

La opción **3** del menú genera automáticamente el código DOT y la imagen PNG con Graphviz:

```bash
dot -Tpng arbol_b.dot -o arbol_b_canciones.png
```

Requisito: tener instalado [Graphviz](https://graphviz.org/).

## Menú del sistema

1. Insertar canción
2. Cargar JSON
3. Cargar CSV
4. Eliminar canción
5. Graficar árbol
6. Salir

- **1. Insertar canción**: pide código, nombre y artista, e inserta la canción en el árbol.
- **2. Cargar JSON**: pide la ruta de un archivo JSON e inserta todas las canciones. Incluye un ejemplo listo para probar: `ArbolB/canciones.json`. Si ejecutas desde `Semana_8/ArbolB/`, la ruta es `canciones.json`.
- **3. Cargar CSV**: pide la ruta de un archivo CSV e inserta todas las canciones. Incluye un ejemplo listo para probar: `ArbolB/canciones.csv`. Si ejecutas desde `Semana_8/ArbolB/`, la ruta es `canciones.csv`.
- **4. Eliminar canción**: pide el código de la canción y la elimina del árbol, rebalanceando después (hojas, nodos internos y raíz).
- **5. Graficar árbol**: genera `arbol_b.dot` y `arbol_b_canciones.png`, y abre la imagen.
- **6. Salir**: termina el programa.

## Cargar canciones desde JSON

La opción **2** permite cargar un archivo JSON que contenga un arreglo de objetos con las claves `codigo`, `nombre` y `artista`. Ejemplo:

```json
[
    {
        "codigo": "C01",
        "nombre": "Animals",
        "artista": "Martin Garrix"
    },
    {
        "codigo": "C02",
        "nombre": "Wizard",
        "artista": "Martin Garrix, Jay Hardway"
    }
]
```

El proyecto incluye un ejemplo listo para probar: `ArbolB/canciones.json` con 10 canciones.

## Cargar canciones desde CSV

La opción **3** permite cargar un archivo CSV con el formato `codigo,nombre,artista`. La primera línea se ignora (encabezado). Ejemplo:

```csv
codigo,nombre,artista
C01,Animals,Martin Garrix
C02,Wizard,"Martin Garrix, Jay Hardway"
```

Si el nombre o el artista contienen comas, se deben encerrar entre comillas dobles.

El proyecto incluye un ejemplo listo para probar: `ArbolB/canciones.csv` con 15 canciones.

