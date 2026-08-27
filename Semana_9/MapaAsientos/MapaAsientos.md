# Mapa de Asientos

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

### Ejemplo: Mapa de Asientos

En esta semana, la matriz representa un **mapa de asientos** de un recinto (cine, teatro, avion, etc.):

- **Filas (encabezados verticales)**: numeros de fila (`1`, `2`, `3`, ...).
- **Columnas (encabezados horizontales)**: numeros de columna (`1`, `2`, `3`, ...).
- **Valor de la celda**: `---` (disponible) o el nombre de la persona que ocupa el asiento.

```
           C1      C2      C3      C4
F1        ---     Juan    ---     Maria
F2        ---     ---     Pedro   ---
F3        Ana     ---     ---     Luis
```

Estructura interna:

```
head
 ├── (encabezado columna C1)
 ├── (encabezado columna C2)
 ├── (encabezado columna C3)
 └── (encabezado columna C4)

 ├── (encabezado fila F1) ──► (F1, C1, ---) ──► (F1, C2, Juan) ──► (F1, C3, ---) ──► (F1, C4, Maria)
 ├── (encabezado fila F2) ──► (F2, C1, ---) ──► (F2, C2, ---) ──► (F2, C3, Pedro) ──► (F2, C4, ---)
 └── (encabezado fila F3) ──► (F3, C1, Ana)  ──► (F3, C2, ---) ──► (F3, C3, ---) ──► (F3, C4, Luis)
```

### Diferencia con la matriz de Semana_5

La matriz de la Semana_5 solo crea nodos cuando hay valores (peliculas). En esta implementacion, **todos los nodos internos se crean al momento de definir las dimensiones**, ya que el mapa de asiento necesita mostrar todas las posiciones posibles, tanto las vacias como las ocupadas. Esto permite que el grafo Graphviz muestre **todos los asientos** sin importar si estan ocupados o no.

## Estructura del proyecto

| Archivo              | Contenido                                                                |
|----------------------|--------------------------------------------------------------------------|
| `MapaAsientos.cpp`   | Clase `Node`, clase `MapaAsientos` y `main()`                           |
| `MapaAsientos.exe`   | Ejecutable compilado con `g++`                                           |

## Como funciona

1. **`crearMapa(filas, columnas)`**: crea la matriz completa con todas las posiciones vacias (`---`). Genera los encabezados de fila y columna, y luego crea un nodo interno por cada posicion de la grilla.

2. **`verMapa()`**: imprime en consola el mapa de asientos como una tabla formateada con los numeros de fila y columna, mostrando `---` para asientos disponibles y el nombre de la persona para asientos ocupados.

3. **`graficar()`**: genera el archivo `mapa_asientos.dot` con el codigo DOT de la matriz y lo convierte en `mapa_asientos.png` usando Graphviz. **Todos los nodos internos aparecen en el grafo**, con colores diferenciados: gris para asientos vacios y verde para asientos ocupados.

## Compilacion y ejecucion

### Compilar desde Semana_9

```bash
cd Semana_9
g++ MapaAsientos/MapaAsientos.cpp -o MapaAsientos/MapaAsientos
./MapaAsientos/MapaAsientos
```

### Compilar desde la raiz del repositorio

```bash
g++ Semana_9/MapaAsientos/MapaAsientos.cpp -o Semana_9/MapaAsientos/MapaAsientos
./Semana_9/MapaAsientos/MapaAsientos
```

> **Nota:** En Windows el ejecutable se genera como `MapaAsientos.exe`. Ejecutar desde la carpeta `Semana_9/MapaAsientos/` para que los archivos `mapa_asientos.dot` y `mapa_asientos.png` se generen dentro de la carpeta `MapaAsientos/`.

## Reporte grafico

La opcion **3** del menu genera automaticamente el codigo DOT y la imagen PNG con Graphviz:

```bash
dot -Tpng mapa_asientos.dot -o mapa_asientos.png
```

Requisito: tener instalado [Graphviz](https://graphviz.org/).

## Menu del sistema

```
========== M E N U ===========
1. Crear mapa de asientos
2. Ver mapa de asientos
3. Graficar matriz
4. Salir
```

- **1. Crear mapa de asientos**: pide el numero de filas y columnas, y crea la matriz completa con todos los asientos disponibles (`---`).
- **2. Ver mapa de asientos**: muestra en consola el mapa de asientos formateado como una tabla, con los numeros de fila y columna como encabezados.
- **3. Graficar matriz**: genera `mapa_asientos.dot` y `mapa_asientos.png` dentro de la carpeta `MapaAsientos/`. El grafo muestra **todos los nodos internos** de la matriz, con color gris para asientos vacios y verde para ocupados.
- **4. Salir**: termina el programa.
