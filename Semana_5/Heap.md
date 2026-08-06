# Heap

## ¿Qué es un heap binario?

Es una estructura de datos que se representa sobre un **arreglo** y cumple la **propiedad de heap**: cada nodo es mayor (o menor) que sus hijos. En este ejemplo se implementa un **Max-Heap de pilotos**, donde la raíz siempre es el piloto con **más horas de vuelo**.

### Representación con arreglo

No se usan punteros: el arreglo guarda el árbol en orden por niveles.

```
              Pedro (850h)      arreglo: [Pedro(850), Luis(600), Carlos(700), ...]
              /        \
        Luis(600h)   Carlos(700h)   hijo izquierdo de i: 2*i + 1
        /    \        /     \       hijo derecho de i:   2*i + 2
       ...   ...     ...    ...     padre de i:          (i-1)/2
```

### Operaciones

- **Insertar**: agrega el piloto al final del arreglo y lo "flota" (sube) hasta su posición correcta según sus horas de vuelo.
- **Heapify**: construye el heap desde un arreglo existente hundiendo cada nodo desde el último padre hacia la raíz (de abajo hacia arriba). Es el algoritmo estándar que usa la carga de CSV.
- **Eliminar máximo**: intercambia la raíz (piloto con más horas) con el último, lo elimina y "hunde" la nueva raíz.
- **Heap sort**: extrae el máximo repetidamente sobre una copia, devolviendo los pilotos ordenados de mayor a menor horas de vuelo.

## Estructura del proyecto

| Archivo    | Contenido                                        |
|------------|--------------------------------------------------|
| `Heap.cpp` | Clase `HeapMaxPilotos`, menú y lógica de la interfaz |
| `pilotos.csv`| Ejemplo de pilotos para la opción de cargar CSV |

## Compilación y ejecución

### Compilar desde Semana_5

```bash
cd Semana_5
g++ heap/Heap.cpp -o heap/Heap
./heap/Heap
```

### Compilar desde la raíz del repositorio

```bash
g++ Semana_5/heap/Heap.cpp -o Semana_5/heap/Heap
./Semana_5/heap/Heap
```

> **Nota:** En Windows el ejecutable se genera como `Heap.exe`. Ejecutar desde la carpeta `Semana_5/` para que los archivos `Heap.dot` y `Heap.png` se generen en el lugar correcto.

## Reporte gráfico

La opción **6** del menú genera automáticamente el código DOT y la imagen PNG con Graphviz:

```bash
dot -Tpng Heap.dot -o heap_de_pilotos.png
```

Requisito: tener instalado [Graphviz](https://graphviz.org/).

## Cargar pilotos desde CSV

La opción **5** permite cargar un archivo CSV con el formato `nombre,nacionalidad,horas_de_vuelo`. Ejemplo:

```csv
nombre,nacionalidad,horas_de_vuelo
Juan Perez,Guatemala,500
Maria Lopez,El Salvador,300
```

El proyecto incluye un ejemplo listo para probar: `pilotos.csv` (los mismos pilotos que el ejemplo del AVL de la Semana 6, para comparar ambas estructuras).

## Menú del sistema

1. Insertar piloto
2. Eliminar máximo
3. Ver pilotos
4. Heap sort
5. Cargar CSV
6. Generar reporte de pilotos
7. Salir

- **1. Insertar piloto**: pide nombre, nacionalidad y horas de vuelo, y lo inserta en el heap.
- **2. Eliminar máximo**: elimina y muestra el piloto con más horas de vuelo (la raíz).
- **3. Ver pilotos**: imprime el arreglo y las relaciones padre-hijo con cada piloto.
- **4. Heap sort**: imprime los pilotos ordenados de mayor a menor horas de vuelo.
- **5. Cargar CSV**: carga los pilotos desde un archivo CSV y construye el heap con el algoritmo estándar de **heapify**.
- **6. Generar reporte de pilotos**: genera `Heap.dot` y `heap_de_pilotos.png` con Graphviz.
- **7. Salir**: termina el programa.
