#include <iostream>
using namespace std;

int main() {
    int nota = 95;

    // if evalua una condicion: si es true ejecuta el bloque
    // else if permite encadenar mas condiciones si la anterior fue false
    // else captura todo lo que no cumplio ninguna condicion anterior
    if (nota >= 90) {
        cout << "Excelente" << endl;
    } else if (nota >= 70) {
        cout << "Bueno" << endl;
    } else if (nota >= 50) {
        cout << "Regular" << endl;
    } else {
        cout << "Reprobado" << endl;
    }

    int numero = 20;

    // % es el operador modulo, retorna el residuo de una division
    // si numero % 2 == 0 significa que es divisible entre 2, o sea par
    if (numero % 2 == 0) {
        cout << numero << " es par" << endl;
    } else {
        cout << numero << " es impar" << endl;
    }

    return 0;
}
