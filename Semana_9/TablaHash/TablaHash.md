# Tabla Hash de Canciones por Nombre del Artista

## ¿Qué es una tabla hash?

Es una estructura de datos que guarda elementos en un **arreglo de tamaño fijo** y usa una **función hash** para decidir en qué posición (índice) va cada elemento. A las posiciones del arreglo se les llama **cubos** o **buckets**. La ventaja: buscar, insertar y eliminar se hacen, en promedio, en **O(1)** porque con la función calculamos el índice y vamos directo al bucket, sin recorrer toda la estructura.

Este ejemplo guarda **canciones por artista**: cada canción tiene el **nombre del artista** (la clave) y la **edad del artista en años** (el dato que lo acompaña).

## La clave es un STRING: el nombre del artista

A diferencia de las duraciones (números), aquí la clave es texto. La función hash trabaja **letra por letra** sobre el nombre del artista. Usamos **un solo método, la POLINÓMICA mod m**:

| Método | Descripción |
|--------|-------------|
| **Polinómica mod m** (regla de Horner, base 31) | Trata el nombre como un polinomio evaluado con la regla de Horner, base 31. En cada paso ya vamos haciendo `% tamano` (módulo) para que el número no crezca sin control. Por ejemplo: `"Shakira"` → S(83), luego (83*31+S(83)=2656)... hasta llegar a un índice entre 0 y 12. |

**Nota:** como la tabla es fija (13 buckets) y los nombres son texto, la función polinómica está pensada para repartir los índices lo mejor posible y reducir colisiones.

## Tamaño fijo y colisiones

La tabla es de **tamaño fijo igual a 13 buckets** (constante en el código). Los índices siempre quedan entre `0` y `12`.

Cuando dos artistas caen en el mismo bucket decimos que hubo una **colisión** y la resolvemos por **encadenamiento** (separate chaining): cada bucket es una **lista enlazada** donde van los nombres que colisionan.

Ejemplo: con DJB2, si `"Shakira"` y otro artista cayeran en el mismo bucket:

```
[3] -> Shakira(47) -> Bad Bunny(30)
```

## Operaciones del menú

```
========== M E N U ===========
1. Insertar canción (artista y edad)
2. Buscar canciones por artista
3. Eliminar canción por artista
4. Ver tabla
5. Graficar tabla
6. Cargar canciones desde CSV
7. Salir
```

1. **Insertar canción**: pide el **nombre del artista** y su **edad** en años, calcula el índice con la función hash sobre el nombre e inserta al inicio del bucket.
2. **Buscar por artista**: pide un nombre y muestra todas las canciones de ese artista (puede haber varias por colisiones).
3. **Eliminar por artista**: quita la primera coincidencia de ese nombre del bucket.
4. **Ver tabla**: muestra cada bucket con su lista.
5. **Graficar tabla**: genera el `.dot` y lo convierte a imagen con Graphviz (estilo Spotify de la Semana 4).
6. **Cargar desde CSV**: lee un CSV `artista,edad` (opción 6) e inserta todas las canciones.
7. **Salir**.

## Archivo CSV de ejemplo

El proyecto incluye `canciones.csv` con 15 canciones de ejemplo. Formato: `artista,edad`. Si el nombre del artista contiene comas, va entre comillas dobles: `"Ricky Martin",52`. La primera línea (encabezado) se ignora automáticamente.

## Compilación y ejecución

```bash
cd Semana_9
g++ TablaHash/TablaHash.cpp -o TablaHash/TablaHash.exe
./TablaHash/TablaHash.exe
```

Para el gráfico se necesita [Graphviz](https://graphviz.org/) (`dot -Tpng TablaHash/tabla_hash.dot -o TablaHash/tabla_hash.png`).

## Archivos del proyecto

| Archivo          | Contenido                                                     |
|------------------|---------------------------------------------------------------|
| `TablaHash.cpp`  | Clase `Nodo` + clase `TablaHash` y `main()`                   |
| `TablaHash.exe`  | Ejecutable compilado con `g++`                                |
| `canciones.csv`  | Ejemplo de 15 canciones para cargar                          |
| `tabla_hash.dot` | Código DOT generado por la opción 5                           |
| `tabla_hash.png` | Imagen de la tabla generada con Graphviz                      |
| `TablaHash.md`   | Esta documentación                                            |


## validar hash

https://meskeia.com/simulador-hashing-colisiones/


## Validación del grafo

Para comprobar que el archivo `tabla_hash.dot` que genera la opción **5** es correcto (sintaxis válida y estructura esperada de buckets y cadenas), se puede validar en línea pegando el contenido del `.dot` en el siguiente enlace:

**Simulador de hashing y colisiones** – [meskeia.com/simulador-hashing-colisiones/](https://meskeia.com/simulador-hashing-colisiones/)

Describe el enlace así en el reporte: *punto de validación del diagrama de la tabla hash para confirmar que el DOT generado por el programa compila y renderiza igual que la imagen local.*