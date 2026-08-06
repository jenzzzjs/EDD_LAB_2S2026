# Matriz Dispersa

## ¿Qué es una matriz dispersa?

Es una estructura de datos que representa una matriz en la que **la mayoria de sus celdas estan vacias**. En lugar de guardar todos los valores en un arreglo bidimensional (desperdiciando memoria), solo se almacenan los datos que realmente existen, conectados entre si con punteros.

### Implementacion con lista ortogonal

La matriz dispersa se implementa como una **lista ortogonal**: cada nodo de la matriz tiene cuatro punteros que lo conectan con sus vecinos:

```
Node
    ├── up     (arriba)
    ├── down   (abajo)
    ├── left   (izquierda)
    └── right  (derecha)
```

### Ejemplo: Matriz de Tipo y Director (Peliculas)

En el ejemplo de esta semana, la matriz guarda la relacion entre el tipo de pelicula y su director:

- **Filas (encabezados verticales)**: tipo de pelicula (`Accion`, `Comedia`, `Drama`, `Terror`, ...).
- **Columnas (encabezados horizontales)**: director (`Nolan`, `Villeneuve`, `Spielberg`, ...).
- **Valor de la celda**: nombre de la pelicula.

```
                    Nolan        Villeneuve    Spielberg
Accion              Batman       Dune
Drama                                            E.T.
Ciencia Ficcion     Interstellar
Comedia             Anchorman
```

Estructura interna:

```
head
 ├── (encabezado fila Accion)          ──► (Accion, Nolan, Batman)
 ├── (encabezado fila Drama)           ──► (Drama, Spielberg, E.T.)
 └── (encabezado fila Ciencia Ficcion) ──► (Ciencia Ficcion, Nolan, Interstellar)
```

## Estructura del proyecto

| Archivo       | Contenido                                                        |
|---------------|------------------------------------------------------------------|
| `Matriz.cpp`  | Clase `Node`, clase `MatrizDispersaTipoDirector` y `main()`      |

## Como funciona

1. **`createNode(row, col, val)`**: inserta una celda en la matriz. Si la fila (tipo) o la columna (director) no existen, crea sus encabezados y los ordena. Luego enlaza la nueva celda con sus vecinos (derecha, izquierda, arriba, abajo).

2. **`printTipoAndDirectorByPelicula(pelicula)`**: recorre toda la matriz buscando el nodo cuyo valor sea igual al nombre de la pelicula e imprime el tipo y el director.

3. **`eliminarPelicula(pelicula)`**: busca la celda con esa pelicula, la desenlaza de la matriz y la elimina. Los encabezados de tipo y director se conservan aunque queden sin peliculas.

4. **`ObtenerGraphviz()`**: genera el archivo `MatrizDispersa.dot` con el codigo DOT de la matriz y lo convierte en `MatrizDispersa.png` usando Graphviz. Ambos se generan dentro de la carpeta `Matriz/`. Alinea las filas y columnas con `rank=same`, `group` y bordes invisibles, muestra flechas visibles entre cada pelicula y su tipo (fila) y su director (columna), encadena las peliculas de un mismo tipo y conecta los tipos entre si y los directores entre si.

## Compilacion y ejecucion

### Compilar desde Semana_5

```bash
cd Semana_5
g++ Matriz/Matriz.cpp -o Matriz/Matriz
./Matriz/Matriz
```

### Compilar desde la raiz del repositorio

```bash
g++ Matriz/Matriz.cpp -o Matriz/Matriz;
./Matriz/Matriz  
```

> **Nota:** En Windows el ejecutable se genera como `Matriz.exe`. Ejecutar desde la carpeta `Semana_5/` para que los archivos `MatrizDispersa.dot` y `MatrizDispersa.png` se generen dentro de la carpeta `Matriz/`.

## Reporte grafico

El programa genera automaticamente el codigo DOT y la imagen PNG con Graphviz:

```bash
dot -Tpng MatrizDispersa.dot -o MatrizDispersa.png
```

Requisito: tener instalado [Graphviz](https://graphviz.org/).

## Datos de ejemplo

El programa tiene un **menu interactivo** con las siguientes opciones:

```
========== M E N U ===========
1. Insertar director
2. Insertar tipo
3. Insertar pelicula
4. Eliminar pelicula
5. Graficar Matriz
6. Salir
```

- **1. Insertar director**: crea el encabezado de columna con el nombre del director.
- **2. Insertar tipo**: crea el encabezado de fila con el tipo de pelicula.
- **3. Insertar pelicula**: pide el director, el tipo de pelicula y el nombre de la pelicula, e inserta la celda en la matriz. Si el tipo (fila) o el director (columna) no existen, se crean automaticamente.
- **4. Eliminar pelicula**: pide el nombre de la pelicula, la desenlaza de la matriz y la elimina. Los tipos y directores se conservan aunque queden sin peliculas.
- **5. Graficar Matriz**: genera `MatrizDispersa.dot` y `MatrizDispersa.png` dentro de la carpeta `Matriz/`.
- **6. Salir**: termina el programa.

Ejemplo de los datos que guarda la matriz:

| Tipo           | Director    | Pelicula       |
|----------------|-------------|----------------|
| `Accion`       | `Nolan`     | `Batman`       |
| `Comedia`      | `McKay`     | `Anchorman`    |
| `Ciencia Ficcion` | `Nolan`  | `Interstellar` |
