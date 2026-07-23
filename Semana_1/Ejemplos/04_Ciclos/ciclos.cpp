#include <iostream>
using namespace std;

int main() {
    // for funciona cuando ya sabes cuantas veces quieres repetir
    // se divide en 3 partes: inicializacion, condicion, incremento
    cout << "--- For ---" << endl;
    for (int i = 1; i <= 5; i++) {
        cout << "i = " << i << endl;
    }

    // while repite mientras la condicion sea true
    // es como un for pero no tienes el incremento obligatorio,
    // asi que tenes que encargarte de actualizar la variable vos
    cout << "\n--- While ---" << endl;
    int contador = 0;
    while (contador < 5) {
        cout << "contador = " << contador << endl;
        contador++;
    }

    // do-while es igual que while pero ejecuta al menos una vez
    // sin importar si la condicion es true o false desde el inicio
    cout << "\n--- Do-While ---" << endl;
    int opcion;
    do {
        cout << "1. Opcion A" << endl;
        cout << "2. Opcion B" << endl;
        cout << "3. Salir" << endl;
        cout << "Elige: ";
        cin >> opcion;
    } while (opcion != 3);

    return 0;
}
