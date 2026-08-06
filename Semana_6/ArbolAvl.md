# Árbol de Pilotos

## ¿Qué es un árbol binario de búsqueda?

Es una estructura jerárquica donde cada nodo tiene a lo sumo **dos hijos** (izquierdo y derecho). La regla del árbol binario de búsqueda es: el hijo izquierdo contiene valores **menores** que el nodo y el hijo derecho valores **mayores**. Esto permite buscar, insertar y eliminar de forma muy eficiente.

En esta semana el árbol guarda a los **pilotos** de un sistema de aviación. Cada piloto tiene 3 campos:

- **Nombre**
- **Nacionalidad**
- **Horas de vuelo**

El árbol se ordena por las **horas de vuelo**:

- **Nodo izquierdo**: piloto con menos horas de vuelo.
- **Nodo derecho**: piloto con más horas de vuelo.

```
             (Piloto C - 700)
             /                \
    (Piloto A - 500)      (Piloto D - 800)
       /       \
(Piloto B - 300)  (Piloto E - 600)
```

## Estructura del proyecto

| Archivo   | Contenido                                                        |
|-----------|------------------------------------------------------------------|
| `Avl.cpp` | Clase `Piloto`, clase `NodoArbolBinarioDeBusquedaPilotos`, clase `ArbolBinarioDeBusquedaPilotos` y `main()` |

## Como funciona

1. **`insertar(piloto)`**: inserta un piloto en el árbol respetando el orden por horas de vuelo. Si las horas de vuelo ya existen, no duplica el nodo.

2. **`inOrden()`, `preOrden()`, `postOrden()`**: recorridos del árbol. `inOrden()` imprime los pilotos **ordenados por horas de vuelo**.

3. **`cargarCSV(ruta)`**: lee un archivo CSV con formato `nombre,nacionalidad,horas_de_vuelo`. La primera línea se ignora (encabezado) y el resto de pilotos se insertan en el árbol.

4. **`buscar(horas_de_vuelo)`**: busca un piloto por sus horas de vuelo. Si no existe, lanza una excepción.

5. **`generarDot()`**: genera el archivo `arbol.dot` con el código DOT del árbol y lo convierte a `arbol_binario_de_pilotos.png` usando Graphviz, luego abre la imagen.

## Compilación y ejecución

### Compilar desde Semana_6

```bash
cd Semana_6
g++ Avl/Avl.cpp -o Avl/Avl
./Avl/Avl
```

### Compilar desde la raíz del repositorio

```bash
g++ Semana_6/Avl/Avl.cpp -o Semana_6/Avl/Avl
./Semana_6/Avl/Avl
```

> **Nota:** En Windows el ejecutable se genera como `Avl.exe`. Ejecutar desde la carpeta `Semana_6/` para que los archivos `arbol.dot` y `arbol_binario_de_pilotos.png` se generen en el lugar correcto.

## Reporte gráfico

La opción **3** del menú genera automáticamente el código DOT y la imagen PNG con Graphviz:

```bash
dot -Tpng arbol.dot -o arbol_binario_de_pilotos.png
```

Requisito: tener instalado [Graphviz](https://graphviz.org/).

## Menú del sistema

1. Insertar piloto
2. Cargar CSV
3. Ver pilotos
4. Generar reporte de pilotos
5. Salir

- **1. Insertar piloto**: pide nombre, nacionalidad y horas de vuelo, e inserta el piloto en el árbol.
- **2. Cargar CSV**: pide la ruta de un archivo CSV (`nombre,nacionalidad,horas_de_vuelo`) e inserta todos los pilotos. Incluye un ejemplo listo para probar: `Avl/pilotos.csv`.
- **3. Ver pilotos**: imprime los pilotos en recorrido in-orden (ordenados por horas de vuelo).
- **4. Generar reporte de pilotos**: genera `arbol.dot` y `arbol_binario_de_pilotos.png`.
- **5. Salir**: termina el programa.

## Cargar pilotos desde CSV

La opción **2** permite cargar un archivo CSV con el formato `nombre,nacionalidad,horas_de_vuelo`. Ejemplo:

```csv
nombre,nacionalidad,horas_de_vuelo
Juan Perez,Guatemala,500
Maria Lopez,El Salvador,300
```

El proyecto incluye un ejemplo listo para probar: `Avl/pilotos.csv`. Si ejecutas desde `Semana_6/`, la ruta es `Avl/pilotos.csv`.
