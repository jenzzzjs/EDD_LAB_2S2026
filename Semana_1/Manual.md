# Semana 1 - Fundamentos de C++

## Ejemplos

### 01. Print
Impresion basica en consola con `cout`.

Desde la raiz de Semana_1:
```bash
g++ -c Ejemplos/01_Print/print.cpp -o Ejemplos/01_Print/print.o
g++ Ejemplos/01_Print/print.o -o Ejemplos/01_Print/print
./Ejemplos/01_Print/print
```

Desde la carpeta del archivo:
```bash
cd Ejemplos/01_Print
g++ -c print.cpp -o print.o
g++ print.o -o print
./print
```

### 02. Variables
Declaracion y uso de distintos tipos de variables (`int`, `double`, `char`, `string`, `bool`).

Desde la raiz de Semana_1:
```bash
g++ -c Ejemplos/02_Variables/variables.cpp -o Ejemplos/02_Variables/variables.o
g++ Ejemplos/02_Variables/variables.o -o Ejemplos/02_Variables/variables
./Ejemplos/02_Variables/variables
```

Desde la carpeta del archivo:
```bash
cd Ejemplos/02_Variables
g++ -c variables.cpp -o variables.o
g++ variables.o -o variables
./variables
```

### 03. Condicionales
Uso de `if`, `else if` y `else` para control de flujo.

Desde la raiz de Semana_1:
```bash
g++ -c Ejemplos/03_Condicionales/condicionales.cpp -o Ejemplos/03_Condicionales/condicionales.o
g++ Ejemplos/03_Condicionales/condicionales.o -o Ejemplos/03_Condicionales/condicionales
./Ejemplos/03_Condicionales/condicionales
```

Desde la carpeta del archivo:
```bash
cd Ejemplos/03_Condicionales
g++ -c condicionales.cpp -o condicionales.o
g++ condicionales.o -o condicionales
./condicionales
```

### 04. Ciclos
Estructuras de repeticion: `for`, `while` y `do-while`.

Desde la raiz de Semana_1:
```bash
g++ -c Ejemplos/04_Ciclos/ciclos.cpp -o Ejemplos/04_Ciclos/ciclos.o
g++ Ejemplos/04_Ciclos/ciclos.o -o Ejemplos/04_Ciclos/ciclos
./Ejemplos/04_Ciclos/ciclos
```

Desde la carpeta del archivo:
```bash
cd Ejemplos/04_Ciclos
g++ -c ciclos.cpp -o ciclos.o
g++ ciclos.o -o ciclos
./ciclos
```

### 05. Funciones
Declaracion y uso de funciones, incluyendo recursion (factorial).

Desde la raiz de Semana_1:
```bash
g++ -c Ejemplos/05_Funciones/funciones.cpp -o Ejemplos/05_Funciones/funciones.o
g++ Ejemplos/05_Funciones/funciones.o -o Ejemplos/05_Funciones/funciones
./Ejemplos/05_Funciones/funciones
```

Desde la carpeta del archivo:
```bash
cd Ejemplos/05_Funciones
g++ -c funciones.cpp -o funciones.o
g++ funciones.o -o funciones
./funciones
```

## Lista Enlazada

### Simple
Lista enlazada simple de estudiantes usando punteros y clases. Opciones: agregar, eliminar por carnet y graficar con Graphviz.

Requiere tener instalado [Graphviz](https://graphviz.org/) para generar el grafico.

**Ejecutar desde la carpeta `ListaEnlazada/Simple/`** para que los archivos `lista.dot` y `lista.png` se generen en el lugar correcto:

```bash
cd ListaEnlazada/Simple
g++ -c lista_enlazada.cpp -o lista_enlazada.o
g++ lista_enlazada.o -o lista_enlazada
./lista_enlazada
```

### Doble
Lista doblemente enlazada de estudiantes. Cada nodo tiene puntero al siguiente y al anterior, permitiendo recorrer en ambas direcciones.

Requiere tener instalado [Graphviz](https://graphviz.org/) para generar el grafico.

**Ejecutar desde la carpeta `ListaEnlazada/Doble/`** para que los archivos `lista.dot` y `lista.png` se generen en el lugar correcto:

```bash
cd ListaEnlazada/Doble
g++ -c lista_doble.cpp -o lista_doble.o
g++ lista_doble.o -o lista_doble
./lista_doble
```
