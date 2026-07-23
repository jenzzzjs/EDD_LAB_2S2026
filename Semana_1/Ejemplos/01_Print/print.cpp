// iostream es la libreria que nos permite imprimir en pantalla y leer datos
#include <iostream>
// Esto es para no tener que escribir std:: cada vez que usamos cout o endl
using namespace std;

// Toda funcion principal se declara con int main()
// es el punto de entrada del programa, lo primero que se ejecuta
int main() {
    // cout es el objeto que imprime texto en consola
    // << es el operador que le indica que enviar a cout
    // endl agrega un salto de linea
    cout << "Hola, Mundo!" << endl;

    // Se pueden concatenar strings con mas texto usando <<
    cout << "Mi nombre es " << "Jens "<<"si sale edd" << endl;

    // cout tambien puede evaluar expresiones aritmeticas directamente
    cout << 3 + 5 << endl;

    // return 0 indica que el programa termino sin errores
    return 0;
}
