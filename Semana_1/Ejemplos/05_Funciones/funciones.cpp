#include <iostream>
using namespace std;

// Las funciones sirven para agrupar codigo que hace algo especifico
// y poder reutilizarlo sin tener que copiar y pegar todo el rato

// Esta retorna un int porque tiene int al inicio, y recibe dos parametros int
int suma(int a, int b) {
    return a + b;
}

// void significa que no retorna nada, solo ejecuta accion
void saludar(string nombre) {
    cout << "Hola, " << nombre << "!" << endl;
}

// Esto es recursion: una funcion que se llama a si misma
// El caso base es cuando n <= 1, ahi para de llamarse
// Si no, se multiplica n por factorial de n-1
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    cout << "Suma: " << suma(3, 5) << endl;
    saludar("Mundo");
    cout << "Factorial de 5: " << factorial(5) << endl;

    return 0;
}
