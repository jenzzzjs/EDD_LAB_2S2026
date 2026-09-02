// ARBOL AVL DE PILOTOS
// Guarda pilotos (nombre, nacionalidad, horas de vuelo) y los ordena
// por horas de vuelo como es un AVL se auto-equilibra para que la
// altura siempre sea la menor posible y las busquedas sean rapidas

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace std;

// funcion creada a mano para no usar la libreria que ya trae el
// maximo y recibe dos numeros y nos regresa cual de los dos es el
// mas grande
int maxInt(int a, int b) {
    // solo comparamos si "a" es mayor que "b" devolvemos "a" y si no
    // devolvemos "b"
    return (a > b) ? a : b;
}

// funcion creada a mano para no usar el atoi de la libreria cstdlib
// y lo que hace es leer un texto que contiene numeros por ejemplo
// "500" y lo convierte en el numero entero 500
int convertirAEntero(const std::string& s) {
    // aqui definimos primero una variable numerica de tipo entero
    // llamada "resultado" en donde iremos acumulando el numero y la
    // inicializamos en 0
    int resultado = 0;
    // luego una variable "i" que inicia en 0 y servira para recorrer
    // el texto de izquierda a derecha posicion por posicion
    int i = 0;
    // tambien definimos un booleano "negativo" en false que se
    // pondra en true si el numero trae un signo menos al inicio
    bool negativo = false;

    // revisamos si el texto no esta vacio y si el primer caracter es
    // un signo negativo "-" y si es asi marcamos que es negativo y
    // empezamos a leer desde la posicion 1 para saltarnos ese signo
    if (!s.empty() && s[0] == '-') {
        negativo = true;
        i = 1;
    }

    // este ciclo recorre el texto caracter por caracter y en cada
    // vuelta toma el caracter actual lo transforma a su valor numero
    // restandole el caracter '0' y lo va sumando al resultado
    // multiplicado por 10 que es la forma de ir construyendo el
    // numero digito por digito
    for (; i < s.size(); i++) {
        // si en algun momento nos encontramos un caracter que no es
        // un digito del 0 al 9 dejamos de leer y salimos del ciclo
        if (s[i] < '0' || s[i] > '9') break;
        // movemos lo que ya teniamos un lugar a la izquierda y le
        // añadimos el nuevo digito
        resultado = resultado * 10 + (s[i] - '0');
    }

    // al final si era un numero negativo le cambiamos el signo al
    // resultado y si no lo devolvemos tal cual
    return negativo ? -resultado : resultado;
}

// clase que representa a cada piloto del sistema y cada piloto
// guarda tres datos su nombre su nacionalidad y sus horas de vuelo
// como es una clase sencilla sus campos son privados y se acceden
// por medio de metodos getter
class Piloto {
private:
    // los tres atributos privados de un piloto y al ser privados solo
    // los metodos de la propia clase pueden tocarlos
    std::string nombre;
    std::string nacionalidad;
    int horas_de_vuelo;

public:
    // constructor que es el metodo que se llama para crear un piloto
    // nuevo recibe los tres datos y se los asigna a los atributos y
    // los signos ":" son la lista de inicializacion que es solo una
    // forma de darles valor a los atributos al momento de crear el
    // objeto
    Piloto(const std::string& nombre, const std::string& nacionalidad, int horas_de_vuelo)
        : nombre(nombre), nacionalidad(nacionalidad), horas_de_vuelo(horas_de_vuelo) {}

    // getters que son metodos que nos permiten leer cada dato del
    // piloto desde afuera de la clase ya que los campos son privados
    // y cada uno devuelve un solo atributo
    std::string getNombre() const { return nombre; }
    std::string getNacionalidad() const { return nacionalidad; }
    int getHorasDeVuelo() const { return horas_de_vuelo; }

    // declaramos como amiga al operador << para que la funcion de
    // impresion definida abajo pueda acceder a los campos privados
    // del piloto
    friend std::ostream& operator<<(std::ostream& os, const Piloto& piloto);
};

// sobrecarga del operador << que nos permite imprimir un piloto
// directamente con cout y en vez de escribir campo por campo solo
// hacemos "cout << piloto" y esta funcion decide que se muestra
std::ostream& operator<<(std::ostream& os, const Piloto& piloto) {
    // aqui escribimos en la salida "os" el nombre la nacionalidad y
    // las horas de vuelo separados por comas para que se vea ordenado
    os << "Nombre: " << piloto.nombre << ", Nacionalidad: " << piloto.nacionalidad
        << ", Horas de Vuelo: " << piloto.horas_de_vuelo;
    // devolvemos el flujo "os" para poder seguir encadenando
    return os;
}

// clase que es la "pieza" con la que se arma el arbol y cada nodo
// guarda un piloto un puntero a su hijo izquierdo un puntero a su
// hijo derecho y el valor de altura que el avl usa para saber si
// esta equilibrado
class NodoArbolBinarioDeBusquedaPilotos {
private:
    Piloto piloto;                                  // el piloto que guarda este nodo
    NodoArbolBinarioDeBusquedaPilotos* izq;         // hijo izquierdo (menos horas)
    NodoArbolBinarioDeBusquedaPilotos* der;         // hijo derecho (mas horas)
    int altura;                                     // altura del nodo dentro del arbol

public:
    // constructor del nodo que recibe el piloto y lo guarda y los
    // hijos inician en nullptr que significa que apuntan a nada
    // porque todavia no tienen hijos y la altura empieza en 1
    NodoArbolBinarioDeBusquedaPilotos(const Piloto& piloto)
        : piloto(piloto), izq(nullptr), der(nullptr), altura(1) {}

    // getters del nodo que leen el piloto el hijo izquierdo el hijo
    // derecho y la altura
    Piloto getPiloto() const { return piloto; }
    NodoArbolBinarioDeBusquedaPilotos* getIzq() const { return izq; }
    NodoArbolBinarioDeBusquedaPilotos* getDer() const { return der; }
    int getAltura() const { return altura; }

    // setters del nodo que nos permiten modificar el piloto apuntar
    // los hijos a otros nodos y actualizar la altura
    void setPiloto(const Piloto& piloto) { this->piloto = piloto; }
    void setIzq(NodoArbolBinarioDeBusquedaPilotos* izq) { this->izq = izq; }
    void setDer(NodoArbolBinarioDeBusquedaPilotos* der) { this->der = der; }
    void setAltura(int altura) { this->altura = altura; }
};

// clase donde esta todo el funcionamiento del arbol y es una clase
// avl completa que tiene metodos publicos que son los que el usuario
// puede llamar y metodos privados o auxiliares que son los que
// trabajan por dentro con los nodos
class ArbolBinarioDeBusquedaPilotos {
private:
    // puntero a la raiz del arbol que es al primer nodo
    NodoArbolBinarioDeBusquedaPilotos* raiz;

    // metodos privados auxiliares que hacen el trabajo pesado por
    // dentro y son privados porque el usuario final no los necesita
    // y la clase los usa internamente
    NodoArbolBinarioDeBusquedaPilotos* insertarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, const Piloto& piloto);
    int alturaNodo(NodoArbolBinarioDeBusquedaPilotos* nodo);
    int factorEquilibrio(NodoArbolBinarioDeBusquedaPilotos* nodo);
    NodoArbolBinarioDeBusquedaPilotos* rotacionDerecha(NodoArbolBinarioDeBusquedaPilotos* nodo);
    NodoArbolBinarioDeBusquedaPilotos* rotacionIzquierda(NodoArbolBinarioDeBusquedaPilotos* nodo);
    NodoArbolBinarioDeBusquedaPilotos* buscarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, int horas_de_vuelo);
    void preOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void inOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void postOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo);
    void generarDotAux(NodoArbolBinarioDeBusquedaPilotos* nodo, std::ofstream& archivo);
    void eliminarNodos(NodoArbolBinarioDeBusquedaPilotos* nodo);
    std::string quitarComillas(const std::string& campo);

public:
    // metodos publicos que son los que el programa llama desde el
    // menu
    ArbolBinarioDeBusquedaPilotos();
    bool estaVacio();
    void insertar(const Piloto& piloto);
    Piloto buscar(int horas_de_vuelo);
    void cargarCSV(const std::string& ruta);
    void preOrden();
    void inOrden();
    void postOrden();
    void generarDot();
    ~ArbolBinarioDeBusquedaPilotos();
};

// constructor del arbol que simplemente inicializa la raiz en
// nullptr osea al inicio el arbol no tiene ningun nodo
ArbolBinarioDeBusquedaPilotos::ArbolBinarioDeBusquedaPilotos() {
    raiz = nullptr;
}

// nos dice si el arbol esta vacio o no y lo hace revisando si la
// raiz apunta a nullptr y si es asi no hay nodos entonces esta
// vacio
bool ArbolBinarioDeBusquedaPilotos::estaVacio() {
    return raiz == nullptr;
}

// metodo publico que recibe un piloto desde afuera y si el arbol
// esta vacio el piloto se vuelve la raiz y si no se llama al metodo
// privado insertarNodo para que lo coloque en su lugar correcto y
// de paso equilibre el arbol
void ArbolBinarioDeBusquedaPilotos::insertar(const Piloto& piloto) {
    // si el arbol esta vacio este piloto es el primero y se convierte
    // en la raiz
    if (estaVacio()) {
        raiz = new NodoArbolBinarioDeBusquedaPilotos(piloto);
    }
    // si no esta vacio mandamos a colocarlo con el metodo auxiliar
    else {
        raiz = insertarNodo(raiz, piloto);
    }
}

// metodo de apoyo que nos devuelve la altura de un nodo y si el nodo
// que le pasamos es nullptr o no existe devuelve 0 porque la altura
// de nada es 0 y esto se usa para no fallar al preguntarle la altura
// a nodos vacios
int ArbolBinarioDeBusquedaPilotos::alturaNodo(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    // si el nodo es nulo la altura es 0
    if (nodo == nullptr) {
        return 0;
    }
    // si existe devolvemos su altura guardada
    return nodo->getAltura();
}

// calcula el equilibrio de un nodo y la regla es a la altura del hijo
// izquierdo le restamos la altura del hijo derecho y si el resultado
// esta entre -1 y 1 el nodo esta balanceado y si es mayor a 1 o
// menor a -1 hay que rotar para arreglar el arbol
int ArbolBinarioDeBusquedaPilotos::factorEquilibrio(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    // si el nodo no existe no hay equilibrio que calcular
    if (nodo == nullptr) {
        return 0;
    }
    // restamos altura izquierda menos altura derecha
    return alturaNodo(nodo->getIzq()) - alturaNodo(nodo->getDer());
}

// una de las operaciones para equilibrar el avl y se usa cuando el
// arbol se carga demasiado hacia la izquierda y toma el hijo
// izquierdo y lo sube para que sea el padre del otro nodo como girar
// los nodos hacia la derecha para repartir mejor el peso
NodoArbolBinarioDeBusquedaPilotos* ArbolBinarioDeBusquedaPilotos::rotacionDerecha(NodoArbolBinarioDeBusquedaPilotos* y) {
    // guardamos el hijo izquierdo de y que llamamos x y el hijo
    // derecho de x que llamamos t2 porque t2 se va a reposicionar
    NodoArbolBinarioDeBusquedaPilotos* x = y->getIzq();
    NodoArbolBinarioDeBusquedaPilotos* T2 = x->getDer();

    // hacemos el giro y x pasa a tener a y como hijo derecho y y pasa
    // a tener a t2 como hijo izquierdo
    x->setDer(y);
    y->setIzq(T2);

    // despues del giro las alturas cambian asi que las recalculamos
    // usando el maximo entre las alturas de los nuevos hijos mas 1
    y->setAltura(maxInt(alturaNodo(y->getIzq()), alturaNodo(y->getDer())) + 1);
    x->setAltura(maxInt(alturaNodo(x->getIzq()), alturaNodo(x->getDer())) + 1);

    // devolvemos a x que ahora es el que quedo arriba como la nueva
    // raiz de esta subseccion
    return x;
}

// el caso espejo de la rotacion derecha y se usa cuando el arbol se
// carga demasiado hacia la derecha y esta vez tomamos el hijo derecho
// y lo subimos para que sea el padre del otro nodo girando hacia la
// izquierda
NodoArbolBinarioDeBusquedaPilotos* ArbolBinarioDeBusquedaPilotos::rotacionIzquierda(NodoArbolBinarioDeBusquedaPilotos* x) {
    // guardamos el hijo derecho de x llamado y y el hijo izquierdo de
    // y llamado t2 que va a cambiar de lugar
    NodoArbolBinarioDeBusquedaPilotos* y = x->getDer();
    NodoArbolBinarioDeBusquedaPilotos* T2 = y->getIzq();

    // hacemos el giro y toma a x como hijo izquierdo y x toma a t2
    // como hijo derecho
    y->setIzq(x);
    x->setDer(T2);

    // recalculamos las alturas de ambos nodos despues del giro
    x->setAltura(maxInt(alturaNodo(x->getIzq()), alturaNodo(x->getDer())) + 1);
    y->setAltura(maxInt(alturaNodo(y->getIzq()), alturaNodo(y->getDer())) + 1);

    // devolvemos a y que es el que quedo arriba ahora
    return y;
}

// aqui esta la logica principal de la insercion y recorre el arbol
// buscando el lugar correcto para el piloto segun sus horas de vuelo
// lo inserta actualiza la altura y verifica si el arbol quedo
// desbalanceado y si lo quedo hace las rotaciones necesarias para
// equilibrarlo y es recursivo
NodoArbolBinarioDeBusquedaPilotos* ArbolBinarioDeBusquedaPilotos::insertarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, const Piloto& piloto) {
    // si el nodo actual es nullptr encontramos el hueco donde va el
    // piloto nuevo asi que creamos un nodo nuevo con el piloto y lo
    // devolvemos
    if (nodo == nullptr) {
        return new NodoArbolBinarioDeBusquedaPilotos(piloto);
    }

    // si el piloto tiene menos horas de vuelo que el nodo actual debe
    // ir del lado izquierdo asi que llamamos a insertarNodo sobre el
    // hijo izquierdo
    if (piloto.getHorasDeVuelo() < nodo->getPiloto().getHorasDeVuelo()) {
        nodo->setIzq(insertarNodo(nodo->getIzq(), piloto));
    }
    // y si tiene mas horas de vuelo debe ir del lado derecho
    else if (piloto.getHorasDeVuelo() > nodo->getPiloto().getHorasDeVuelo()) {
        nodo->setDer(insertarNodo(nodo->getDer(), piloto));
    }
    // si las horas de vuelo son iguales a las de un piloto que ya
    // existe no lo duplicamos y solo devolvemos el nodo tal cual
    else {
        return nodo;
    }

    // ya que insertamos actualizamos la altura de este nodo y la
    // nueva altura es el maximo entre las alturas de sus dos hijos
    // mas uno porque cuenta este nodo
    nodo->setAltura(maxInt(alturaNodo(nodo->getIzq()), alturaNodo(nodo->getDer())) + 1);

    // calculamos el factor de equilibrio de este nodo para saber si
    // quedo balanceado o si hay que rotar
    int balance = factorEquilibrio(nodo);

    // caso izquierda-izquierda si el balance es mayor a 1 y el nuevo
    // piloto se fue al lado izquierdo del hijo izquierdo hacemos una
    // rotacion derecha
    if (balance > 1 && piloto.getHorasDeVuelo() < nodo->getIzq()->getPiloto().getHorasDeVuelo()) {
        return rotacionDerecha(nodo);
    }

    // caso derecha-derecha si el balance es menor a -1 y el piloto se
    // fue al lado derecho del hijo derecho hacemos una rotacion
    // izquierda
    if (balance < -1 && piloto.getHorasDeVuelo() > nodo->getDer()->getPiloto().getHorasDeVuelo()) {
        return rotacionIzquierda(nodo);
    }

    // caso izquierda-derecha balance mayor a 1 pero el piloto se fue
    // al lado derecho del hijo izquierdo y primero rotamos a la
    // izquierda el hijo izquierdo y luego rotamos a la derecha
    if (balance > 1 && piloto.getHorasDeVuelo() > nodo->getIzq()->getPiloto().getHorasDeVuelo()) {
        nodo->setIzq(rotacionIzquierda(nodo->getIzq()));
        return rotacionDerecha(nodo);
    }

    // caso derecha-izquierda balance menor a -1 pero el piloto se fue
    // al lado izquierdo del hijo derecho y rotamos a la derecha el
    // hijo derecho y luego rotamos a la izquierda
    if (balance < -1 && piloto.getHorasDeVuelo() < nodo->getDer()->getPiloto().getHorasDeVuelo()) {
        nodo->setDer(rotacionDerecha(nodo->getDer()));
        return rotacionIzquierda(nodo);
    }

    // si el nodo quedo balanceado lo devolvemos tal cual
    return nodo;
}

// cuando leemos un campo del csv a veces viene envuelto entre
// comillas dobles por ejemplo "Juan" y esta funcion revisa si el
// texto empieza y termina con comilla y si es asi se las quita
// dejando solo el contenido
std::string ArbolBinarioDeBusquedaPilotos::quitarComillas(const std::string& campo) {
    // copiamos el campo a un nuevo texto llamado resultado
    std::string resultado = campo;
    // si tiene al menos 2 caracteres y el primero y el ultimo son
    // comilla entonces quitamos la primera y la ultima letra con
    // substr que recorta el texto desde la posicion 1 y con un largo
    // de resultado.size() - 2
    if (resultado.size() >= 2 && resultado.front() == '"' && resultado.back() == '"') {
        resultado = resultado.substr(1, resultado.size() - 2);
    }
    // devolvemos el texto ya sin comillas o igual si no tenia
    return resultado;
}

// metodo publico que lee un archivo de texto en formato csv donde
// cada linea es un piloto con su nombre nacionalidad y horas de
// vuelo separados por comas y la primera linea que es el encabezado
// se ignora y los pilotos se van insertando en el arbol
void ArbolBinarioDeBusquedaPilotos::cargarCSV(const std::string& ruta) {
    // abrimos el archivo con ifstream y si no se puede abrir porque
    // la ruta esta mal o no existe avisamos y salimos
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo '" << ruta << "'." << std::endl;
        return;
    }

    // definimos una variable linea donde guardaremos cada linea
    // leida un booleano encabezado que inicia en true para saltarnos
    // la primera linea y un contador de pilotos insertados
    std::string linea;
    bool encabezado = true;
    int insertados = 0;

    // este ciclo va leyendo el archivo linea por linea hasta que ya
    // no haya mas y getline nos devuelve cada linea completa
    while (getline(archivo, linea)) {
        // si la linea esta vacia nos la saltamos y seguimos con la
        // siguiente
        if (linea.empty()) {
            continue;
        }

        // si todavia es el encabezado que es la primera linea lo
        // marcamos como ya pasado y continuamos con la siguiente
        if (encabezado) {
            encabezado = false;
            continue;
        }

        // empezamos a separar la linea por comas y inicio guarda
        // donde empieza el campo actual y aqui tendremos el nombre la
        // nacionalidad y el texto de las horas ademas de un contador
        // que nos dice en que campo vamos que puede ser 0 1 o 2
        size_t inicio = 0;
        std::string nombre, nacionalidad, horasTxt;
        int campoActual = 0;

        // este ciclo recorre la linea posicion por posicion hasta el
        // final y usamos i <= linea.size() para capturar tambien el
        // ultimo campo cuando i llega al tamaño de la linea
        for (size_t i = 0; i <= linea.size(); ++i) {
            // si llegamos a una coma o al final de la linea ya
            // terminamos de leer un campo y lo extraemos con substr
            // desde inicio hasta i y le quitamos las comillas
            if (i == linea.size() || linea[i] == ',') {
                std::string campo = quitarComillas(linea.substr(inicio, i - inicio));
                // segun el numero de campo en el que vamos lo
                // guardamos en nombre nacionalidad u horas
                if (campoActual == 0) nombre = campo;
                else if (campoActual == 1) nacionalidad = campo;
                else if (campoActual == 2) horasTxt = campo;
                // avanzamos al siguiente campo y movemos inicio a la
                // posicion despues de la coma
                campoActual++;
                inicio = i + 1;
            }
        }

        // si al final si tenemos minimo 3 campos convertimos el texto
        // de horas a numero entero con nuestra funcion hecha a mano y
        // creamos e insertamos el piloto en el arbol
        if (campoActual >= 3) {
            int horas_de_vuelo = convertirAEntero(horasTxt);
            insertar(Piloto(nombre, nacionalidad, horas_de_vuelo));
            // contamos uno mas como insertado
            insertados++;
        }
    }

    // cerramos el archivo y avisamos cuantos pilotos se cargaron
    archivo.close();
    std::cout << "Se cargaron " << insertados << " pilotos desde '" << ruta << "'." << std::endl;
}

// busca un nodo que tenga ciertas horas de vuelo dentro del arbol y
// aprovecha que es un arbol de busqueda si el numero que buscamos es
// menor al del nodo actual vamos a la izquierda y si es mayor a la
// derecha y es recursivo
NodoArbolBinarioDeBusquedaPilotos* ArbolBinarioDeBusquedaPilotos::buscarNodo(NodoArbolBinarioDeBusquedaPilotos* nodo, int horas_de_vuelo) {
    // si el nodo no existe o si sus horas de vuelo son las que
    // buscamos devolvemos ese nodo o nullptr si no existe
    if (nodo == nullptr || nodo->getPiloto().getHorasDeVuelo() == horas_de_vuelo) {
        return nodo;
    }

    // si lo que buscamos es menor a las horas de este nodo la
    // respuesta solo puede estar a la izquierda asi que buscamos ahi
    if (horas_de_vuelo < nodo->getPiloto().getHorasDeVuelo()) {
        return buscarNodo(nodo->getIzq(), horas_de_vuelo);
    }
    // y si es mayor solo puede estar a la derecha
    else {
        return buscarNodo(nodo->getDer(), horas_de_vuelo);
    }
}

// el metodo que el usuario llama para buscar un piloto por sus horas
// de vuelo y si lo encuentra devuelve el piloto y si no lanza una
// excepcion avisando que no existe
Piloto ArbolBinarioDeBusquedaPilotos::buscar(int horas_de_vuelo) {
    // llamamos al metodo privado que recorre el arbol empezando por
    // la raiz
    NodoArbolBinarioDeBusquedaPilotos* nodo = buscarNodo(raiz, horas_de_vuelo);
    // si el nodo existe devolvemos el piloto que tiene
    if (nodo != nullptr) {
        return nodo->getPiloto();
    }
    // si no existe lanzamos una excepcion de tipo runtime_error con
    // un mensaje que diga que no se encontro
    else {
        throw std::runtime_error("Piloto no encontrado");
    }
}

// realiza el recorrido en pre-orden y la regla es primero se visita
// el nodo actual luego el hijo izquierdo y luego el hijo derecho y
// es recursivo
void ArbolBinarioDeBusquedaPilotos::preOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    // si el nodo no es nulo lo recorremos
    if (nodo != nullptr) {
        // primero imprimimos el piloto de este nodo
        std::cout << nodo->getPiloto() << std::endl;
        // luego recorremos todo el lado izquierdo
        preOrdenAux(nodo->getIzq());
        // y despues todo el lado derecho
        preOrdenAux(nodo->getDer());
    }
}

// simplemente inicia el recorrido pre-orden desde la raiz llamando al
// metodo auxiliar
void ArbolBinarioDeBusquedaPilotos::preOrden() {
    preOrdenAux(raiz);
}

// realiza el recorrido en in-orden y la regla es primero se visita el
// hijo izquierdo luego el nodo actual y luego el hijo derecho y como
// el arbol guarda los menores a la izquierda este recorrido nos
// imprime los pilotos ordenados de menor a mayor por horas de vuelo
void ArbolBinarioDeBusquedaPilotos::inOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    // si el nodo no es nulo lo recorremos
    if (nodo != nullptr) {
        // primero todo el lado izquierdo que son los de menos horas
        inOrdenAux(nodo->getIzq());
        // luego el nodo actual
        std::cout << nodo->getPiloto() << std::endl;
        // y al final todo el lado derecho que son los de mas horas
        inOrdenAux(nodo->getDer());
    }
}

// inicia el recorrido in-orden desde la raiz
void ArbolBinarioDeBusquedaPilotos::inOrden() {
    inOrdenAux(raiz);
}

// realiza el recorrido en post-orden y la regla es primero se visita
// el hijo izquierdo luego el hijo derecho y al final el nodo actual
void ArbolBinarioDeBusquedaPilotos::postOrdenAux(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    // si el nodo no es nulo lo recorremos
    if (nodo != nullptr) {
        // primero todo el lado izquierdo
        postOrdenAux(nodo->getIzq());
        // luego todo el lado derecho
        postOrdenAux(nodo->getDer());
        // y al final imprimimos el nodo actual
        std::cout << nodo->getPiloto() << std::endl;
    }
}

// inicia el recorrido post-orden desde la raiz
void ArbolBinarioDeBusquedaPilotos::postOrden() {
    postOrdenAux(raiz);
}

// recorre el arbol y va escribiendo en el archivo dot las conexiones
// o flechas entre cada nodo y sus hijos y esto es lo que despues
// graphviz convierte en la imagen del arbol
void ArbolBinarioDeBusquedaPilotos::generarDotAux(NodoArbolBinarioDeBusquedaPilotos* nodo, std::ofstream& archivo) {
    // si el nodo no es nulo procesamos sus conexiones
    if (nodo != nullptr) {
        // construimos el texto del nombre del nodo actual que incluye
        // el nombre del piloto y sus horas de vuelo separados por un
        // salto de linea y puestos entre comillas
        std::string nombreNodo = "\"" + nodo->getPiloto().getNombre() + "\\n" + std::to_string(nodo->getPiloto().getHorasDeVuelo()) + " horas\"";

        // si el nodo tiene hijo izquierdo escribimos la flecha del
        // nodo actual hacia ese hijo izquierdo
        if (nodo->getIzq() != nullptr) {
            std::string nombreIzq = "\"" + nodo->getIzq()->getPiloto().getNombre() + "\\n" + std::to_string(nodo->getIzq()->getPiloto().getHorasDeVuelo()) + " horas\"";
            archivo << nombreNodo << " -> " << nombreIzq << ";\n";
        }
        // si el nodo tiene hijo derecho escribimos la flecha hacia
        // ese hijo derecho
        if (nodo->getDer() != nullptr) {
            std::string nombreDer = "\"" + nodo->getDer()->getPiloto().getNombre() + "\\n" + std::to_string(nodo->getDer()->getPiloto().getHorasDeVuelo()) + " horas\"";
            archivo << nombreNodo << " -> " << nombreDer << ";\n";
        }
        // recorremos primero todo el lado izquierdo y luego el derecho
        // para generar las conexiones de todos los nodos
        generarDotAux(nodo->getIzq(), archivo);
        generarDotAux(nodo->getDer(), archivo);
    }
}

// crea el archivo arbol.dot con la estructura del arbol y le pide a
// graphviz con el comando dot que convierta ese archivo a una imagen
// png para luego abrirla
void ArbolBinarioDeBusquedaPilotos::generarDot() {
    // abrimos o creamos el archivo arbol.dot
    std::ofstream archivo("arbol.dot");
    // escribimos la cabecera del dot que es el nombre del grafo el
    // color de fondo y la forma de los nodos y las aristas
    archivo << "digraph ArbolBinarioDeBusquedaPilotos {\n";
    archivo << "bgcolor=lightblue;\n";
    archivo << "Node [shape=square, style=filled, fillcolor=beige, color=black, penwidth=2];\n";
    archivo << "edge [splines=polyline, arrowhead=curve];\n";

    // llamamos al metodo auxiliar para que escriba todas las
    // conexiones del arbol
    generarDotAux(raiz, archivo);
    // cerramos el bloque del dot y el archivo
    archivo << "}\n";
    archivo.close();

    // con estos comandos del sistema le decimos a graphviz que
    // convierta arbol.dot a una imagen png y luego la abrimos
    system("dot -Tpng arbol.dot -o arbol_binario_de_pilotos.png");
    system("start arbol_binario_de_pilotos.png");
}

// destructor del arbol que se ejecuta cuando el arbol deja de existir
// y su unico trabajo es liberar toda la memoria de los nodos llamando
// al metodo eliminarNodos sobre la raiz
ArbolBinarioDeBusquedaPilotos::~ArbolBinarioDeBusquedaPilotos() {
    eliminarNodos(raiz);
}

// recorre el arbol en post-orden que es primero los hijos y luego el
// padre y va liberando la memoria de cada nodo con delete y es
// recursivo
void ArbolBinarioDeBusquedaPilotos::eliminarNodos(NodoArbolBinarioDeBusquedaPilotos* nodo) {
    // si el nodo no es nulo lo eliminamos junto con sus hijos
    if (nodo != nullptr) {
        // primero liberamos todo el lado izquierdo
        eliminarNodos(nodo->getIzq());
        // luego todo el lado derecho
        eliminarNodos(nodo->getDer());
        // y al final liberamos este nodo
        delete nodo;
    }
}

// aqui arranca el programa y crea un arbol vacio y muestra un menu
// repetidas veces para que el usuario elija que hacer
int main() {
    // creamos el arbol de pilotos vacio
    ArbolBinarioDeBusquedaPilotos arbol;
    // variable donde guardaremos la opcion que elija el usuario
    int opcion;

    // este ciclo hace que el menu se repita una y otra vez hasta que
    // el usuario elija la opcion 5 que es salir y primero muestra el
    // menu luego pide la opcion y ejecuta la accion segun el case
    do {
        // mostramos el menu en pantalla
        std::cout << std::endl;
        std::cout << "========== M E N U ===========" << std::endl;
        std::cout << "1. Insertar piloto" << std::endl;
        std::cout << "2. Cargar CSV" << std::endl;
        std::cout << "3. Ver pilotos" << std::endl;
        std::cout << "4. Generar reporte de pilotos" << std::endl;
        std::cout << "5. Salir" << std::endl;
        std::cout << "Ingrese una opcion: ";
        // leemos la opcion que escriba el usuario y el ignore sirve
        // para descartar el salto de linea que queda en el buffer
        std::cin >> opcion;
        std::cin.ignore();

        // segun el numero elegido hacemos una cosa u otra
        switch (opcion) {
        case 1: {
            // opcion 1 para insertar un piloto y pedimos nombre
            // nacionalidad y horas de vuelo y creamos el piloto para
            // insertarlo
            std::string nombre, nacionalidad;
            int horas_de_vuelo;

            std::cout << "Ingrese el nombre: ";
            std::getline(std::cin, nombre);
            std::cout << "Ingrese la nacionalidad: ";
            std::getline(std::cin, nacionalidad);
            std::cout << "Ingrese las horas de vuelo: ";
            std::cin >> horas_de_vuelo;
            std::cin.ignore();

            arbol.insertar(Piloto(nombre, nacionalidad, horas_de_vuelo));
            std::cout << "Piloto insertado correctamente." << std::endl;
            break;
        }
        case 2: {
            // opcion 2 para cargar pilotos desde un archivo csv y
            // pedimos la ruta y llamamos a cargarCSV
            std::string ruta;
            std::cout << "Ingrese la ruta del archivo CSV: ";
            std::getline(std::cin, ruta);

            arbol.cargarCSV(ruta);
            break;
        }
        case 3:
            // opcion 3 para ver los pilotos en orden de menor a mayor
            // horas de vuelo usando el recorrido in-orden
            std::cout << "===== PILOTOS (IN ORDEN) =====" << std::endl;
            arbol.inOrden();
            break;
        case 4:
            // opcion 4 para generar el reporte grafico del arbol
            arbol.generarDot();
            std::cout << "Reporte de pilotos generado correctamente." << std::endl;
            break;
        case 5:
            // opcion 5 para salir del programa
            std::cout << "Saliendo del programa..." << std::endl;
            break;
        default:
            // si el usuario escribe un numero que no esta en el menu
            // le avisamos
            std::cout << "Opcion no valida." << std::endl;
            break;
        }

        // el ciclo se repite mientras la opcion no sea 5 que es salir
    } while (opcion != 5);

    // el programa termina devolviendo 0 que significa que todo salio
    // bien
    return 0;
}
