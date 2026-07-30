# Lista Circular

## ¿Qué es una lista circular simple?

Es una variante de la lista enlazada simple donde el último nodo, en lugar de apuntar a `nullptr`, apunta de vuelta al primer nodo (cabeza). Esto forma un ciclo cerrado, lo que permite recorrer la lista de forma continua sin llegar a un final.

### Variantes

- **Lista Circular Simple**: cada nodo tiene un solo puntero (`siguiente`) y el último nodo apunta al primero. El recorrido es en una sola dirección.

- **Lista Circular Doble**: cada nodo tiene dos punteros (`siguiente` y `anterior`), y tanto el primero como el último se conectan entre sí formando un ciclo bidireccional.

## Compilación y ejecución

### Simple
```bash
g++ simple/lista_circular.cpp -o simple/lista_circular
./simple/lista_circular
```

### Doble
```bash
g++ doble/lista_circular_doble.cpp -o doble/lista_circular_doble
./doble/lista_circular_doble
```
