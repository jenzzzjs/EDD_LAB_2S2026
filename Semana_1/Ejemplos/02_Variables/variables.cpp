#include <iostream>
using namespace std;

int main() {
    // int almacena numeros enteros sin decimales
    int edad = 21;

    // double guarda numeros con decimales
    double promedio = 8.5;

    // char es para un solo caracter, se usa comilla simple
    char inicial = 'J';

    // string es para texto, se usa comilla doble, y necesita incluir <string> pero
    // en algunos compiladores iostream ya lo trae incluido
    string nombre = "Jens";

    // bool solo puede ser true o false
    bool aprobado = true;

    cout << "Nombre: " << nombre << endl;
    cout << "Edad: " << edad << endl;
    cout << "Promedio: " << promedio << endl;
    cout << "Inicial: " << inicial << endl;
    cout << "Aprobado: " << aprobado << endl;

    return 0;
}
