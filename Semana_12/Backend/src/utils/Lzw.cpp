#include "Lzw.h"

#include <cstdint>

namespace lzw {
namespace {

// Cada codigo ocupa 12 bits, asi que los codigos van de 0 a 4095. Cuando el
// siguiente codigo llega a 4096 el diccionario esta lleno y hay que reiniciarlo.
const int MAX_CODIGOS = 4096;

// La tabla hash tiene 8191 posiciones, un primo apenas menor que el doble de
// 4096. Asi la carga queda cerca del 50 por ciento y las colisiones son pocas.
const int TAM_TABLA = 8191;

// El codigo 256 no representa un byte. Lo reservamos para avisar al
// descompresor que debe borrar el diccionario y empezar de nuevo.
const int CODIGO_CLEAR = 256;

// Ponemos un limite al texto para no intentar reservar memoria sin control.
const size_t MAX_TEXTO = 128u * 1024u * 1024u;

// Calculamos un hash FNV-1a de 32 bits. Este valor solo sirve para elegir una
// posicion inicial dentro de la tabla del diccionario. No es un hash de
// seguridad, es una ayuda para encontrar secuencias sin recorrer todo.
uint32_t hashCadena(const std::string& s) {
    uint32_t h = 2166136261u;
    for (unsigned char c : s) {
        h ^= c;
        h *= 16777619u;
    }
    return h;
}

// El diccionario guarda dos cosas. En palabras[codigo] esta la secuencia de
// bytes que representa ese codigo. En tabla[hash] esta el codigo de la
// secuencia que cayo en esa posicion. La tabla usa direccionamiento abierto:
// si una posicion esta ocupada, avanzamos a la siguiente hasta encontrar un
// lugar libre. Con esto podemos buscar una secuencia sin recorrer todas.
struct Diccionario {
    std::string* palabras;
    int* tabla;
    int siguiente;

    Diccionario() : palabras(new std::string[MAX_CODIGOS]), tabla(new int[TAM_TABLA]) {
        // Al inicio ninguna posicion de la tabla tiene un codigo.
        for (int i = 0; i < TAM_TABLA; i++) tabla[i] = -1;
        // Los primeros 256 codigos son los bytes literales. Los insertamos
        // para que el compresor pueda encontrar cualquier byte desde el inicio.
        for (int i = 0; i < 256; i++) {
            palabras[i] = std::string(1, static_cast<char>(i));
            insertar(palabras[i], i);
        }
        // El codigo 256 esta reservado para CLEAR, asi que el primer codigo
        // nuevo que podemos usar es el 257.
        siguiente = CODIGO_CLEAR + 1;
    }

    ~Diccionario() {
        delete[] palabras;
        delete[] tabla;
    }

    Diccionario(const Diccionario&) = delete;
    Diccionario& operator=(const Diccionario&) = delete;

    // Cuando el diccionario se llena mandamos CLEAR y lo reiniciamos. Borramos
    // la tabla hash y volvemos a insertar los 256 bytes literales. Las
    // secuencias viejas quedan en palabras, pero ya no se pueden alcanzar
    // porque siguiente vuelve a 257 y solo aceptamos codigos menores.
    void reiniciar() {
        for (int i = 0; i < TAM_TABLA; i++) tabla[i] = -1;
        for (int i = 0; i < 256; i++) insertar(palabras[i], i);
        siguiente = CODIGO_CLEAR + 1;
    }

    // Para buscar una secuencia calculamos su hash y revisamos esa posicion.
    // Si esta ocupada por otra secuencia, avanzamos una posicion y volvemos a
    // comparar. Cuando llegamos a una posicion vacia sabemos que la secuencia
    // no esta en el diccionario.
    int buscar(const std::string& palabra) const {
        uint32_t pos = hashCadena(palabra) % TAM_TABLA;
        while (tabla[pos] != -1) {
            if (palabras[tabla[pos]] == palabra) return tabla[pos];
            pos = (pos + 1) % TAM_TABLA;
        }
        return -1;
    }

    // Para insertar hacemos lo mismo que al buscar, pero nos quedamos con la
    // primera posicion vacia. Guardamos el codigo en la tabla y la secuencia
    // completa en palabras.
    void insertar(const std::string& palabra, int codigo) {
        uint32_t pos = hashCadena(palabra) % TAM_TABLA;
        while (tabla[pos] != -1) pos = (pos + 1) % TAM_TABLA;
        tabla[pos] = codigo;
        palabras[codigo] = palabra;
    }
};

// Guardamos el tamano original en 8 bytes, empezando por el byte menos
// significativo. El descompresor necesita ese numero para reservar memoria y
// para comprobar que el resultado final tiene el tamano correcto.
void escribir64(std::string& salida, uint64_t valor) {
    for (int i = 0; i < 8; i++)
        salida.push_back(static_cast<char>((valor >> (i * 8)) & 0xFF));
}

// Los codigos de LZW ocupan 12 bits, pero los archivos se escriben en bytes de
// 8 bits. Este escritor acumula los bits en un buffer y va sacando bytes
// completos cada vez que junta 8. Al final, si sobran bits, los completa con
// ceros para no perder el ultimo codigo.
struct EscritorBits {
    std::string& salida;
    uint32_t buffer = 0;
    int bits = 0;

    explicit EscritorBits(std::string& s) : salida(s) {}

    void codigo(uint16_t valor) {
        // Metemos los 12 bits del codigo a partir de la posicion libre.
        buffer |= static_cast<uint32_t>(valor) << bits;
        bits += 12;
        // Mientras tengamos al menos 8 bits, escribimos un byte y corremos
        // el resto hacia la derecha.
        while (bits >= 8) {
            salida.push_back(static_cast<char>(buffer & 0xFF));
            buffer >>= 8;
            bits -= 8;
        }
    }

    void cerrar() {
        // Si quedaron bits sin completar un byte, escribimos el ultimo byte.
        if (bits > 0) {
            salida.push_back(static_cast<char>(buffer & 0xFF));
            buffer = 0;
            bits = 0;
        }
    }
};

// Leemos los 8 bytes del tamano original en el mismo orden en que los
// escribimos. Cada byte se coloca en su posicion dentro del entero.
uint64_t leer64(const std::string& datos, size_t pos) {
    uint64_t valor = 0;
    for (int i = 0; i < 8; i++)
        valor |= static_cast<uint64_t>(static_cast<unsigned char>(datos[pos + i])) << (i * 8);
    return valor;
}

}  // namespace

// Comprimimos el texto completo. Primero escribimos el identificador del
// formato y el tamano original. Despues recorremos el texto byte por byte y
// construimos la secuencia mas larga que ya existe en el diccionario.
bool comprimir(const std::string& texto, std::string& salida, std::string& error) {
    error.clear();
    salida.clear();
    if (texto.size() > MAX_TEXTO) {
        error = "el texto es demasiado grande para LZW";
        return false;
    }

    // El archivo empieza con LZW1 y el tamano que tenia el texto sin comprimir.
    salida.append("LZW1", 4);
    escribir64(salida, static_cast<uint64_t>(texto.size()));

    EscritorBits escritor(salida);
    Diccionario dic;
    // En actual guardamos la secuencia mas larga que hemos encontrado en el
    // diccionario mientras leemos el texto.
    std::string actual;

    for (size_t i = 0; i < texto.size(); i++) {
        char c = texto[i];
        // Probamos si la secuencia actual mas el byte nuevo ya existe.
        std::string candidato = actual;
        candidato.push_back(c);

        int codigo = dic.buscar(candidato);
        if (codigo >= 0) {
            // La secuencia existe, asi que la alargamos y seguimos leyendo.
            actual = candidato;
            continue;
        }

        // La secuencia no existe. Escribimos el codigo de la secuencia actual,
        // que si esta en el diccionario porque la encontramos antes.
        int codigoActual = dic.buscar(actual);
        if (codigoActual < 0) {
            error = "estado interno de LZW invalido";
            return false;
        }
        escritor.codigo(static_cast<uint16_t>(codigoActual));

        // Agregamos la secuencia nueva al diccionario para poder usarla mas
        // adelante. Si ya no cabe, mandamos CLEAR y reiniciamos el diccionario.
        if (dic.siguiente < MAX_CODIGOS) {
            dic.insertar(candidato, dic.siguiente);
            dic.siguiente++;
        }
        if (dic.siguiente >= MAX_CODIGOS) {
            escritor.codigo(CODIGO_CLEAR);
            dic.reiniciar();
        }

        // Empezamos una secuencia nueva con el byte que no coincidio.
        actual.assign(1, c);
    }

    // Al terminar el recorrido todavia nos queda una secuencia sin escribir.
    if (!actual.empty()) {
        int codigoActual = dic.buscar(actual);
        if (codigoActual < 0) {
            error = "estado interno de LZW invalido al cerrar";
            return false;
        }
        escritor.codigo(static_cast<uint16_t>(codigoActual));
    }
    // Cerramos el escritor de bits para completar el ultimo byte.
    escritor.cerrar();
    return true;
}

// Descomprimir es el camino inverso. Leemos el identificador y el tamano
// original, reconstruimos el diccionario con los 256 bytes literales y vamos
// leyendo codigos de 12 bits. Cada codigo se convierte en una secuencia de
// bytes que se agrega al texto final. Al mismo tiempo agregamos al diccionario
// la secuencia anterior mas el primer byte de la secuencia actual, que es
// exactamente la entrada que el compresor creo al escribir ese codigo.
bool descomprimir(const std::string& entrada, std::string& texto, std::string& error) {
    error.clear();
    texto.clear();
    // Antes de leer codigos comprobamos que el archivo tenga el formato
    // esperado. Si no empieza con LZW1 no podemos saber como interpretarlo.
    if (entrada.size() < 12 || entrada.compare(0, 4, "LZW1") != 0) {
        error = "archivo LZW invalido (magic incorrecto)";
        return false;
    }
    uint64_t original = leer64(entrada, 4);
    if (original > MAX_TEXTO) {
        error = "el tamano original de LZW es demasiado grande";
        return false;
    }
    texto.reserve(static_cast<size_t>(original));

    // El descompresor empieza con el mismo diccionario que el compresor: los
    // 256 bytes literales. El codigo 256 queda reservado para CLEAR.
    std::string* palabras = new std::string[MAX_CODIGOS];
    for (int i = 0; i < 256; i++)
        palabras[i] = std::string(1, static_cast<char>(i));
    int siguiente = CODIGO_CLEAR + 1;
    // En anterior guardamos la secuencia que se decodifico en la vuelta
    // pasada. La necesitamos para agregar la entrada nueva al diccionario.
    std::string anterior;

    // Los codigos empiezan despues de los 12 bytes del encabezado. Los vamos
    // leyendo de a 12 bits, aunque esten repartidos entre varios bytes.
    size_t pos = 12;
    uint32_t buffer = 0;
    int bits = 0;
    while (true) {
        while (bits < 12 && pos < entrada.size()) {
            buffer |= static_cast<uint32_t>(static_cast<unsigned char>(entrada[pos])) << bits;
            bits += 8;
            pos++;
        }
        if (bits < 12) break;
        uint16_t codigo = static_cast<uint16_t>(buffer & 0x0FFF);
        buffer >>= 12;
        bits -= 12;

        // CLEAR significa que el compresor lleno el diccionario y lo reinicio.
        // Hacemos lo mismo para que ambos lados vuelvan a estar sincronizados.
        if (codigo == CODIGO_CLEAR) {
            siguiente = CODIGO_CLEAR + 1;
            anterior.clear();
            continue;
        }

        std::string entradaDic;
        if (codigo < siguiente && codigo < MAX_CODIGOS) {
            // El codigo ya existe en el diccionario, asi que copiamos su
            // secuencia directamente.
            entradaDic = palabras[codigo];
        } else if (codigo == siguiente && !anterior.empty()) {
            // Este es el caso especial de LZW. El codigo que llega todavia no
            // esta en el diccionario porque el compresor lo creo en el mismo
            // paso. La secuencia es anterior mas el primer byte de anterior.
            entradaDic = anterior;
            entradaDic.push_back(anterior[0]);
        } else {
            delete[] palabras;
            error = "codigo LZW invalido";
            return false;
        }

        // Si el texto crece mas de lo que decia el encabezado, el archivo esta
        // corrupto o no corresponde a ese tamano original.
        if (texto.size() + entradaDic.size() > original) {
            delete[] palabras;
            error = "el texto descomprimido excede el tamano esperado";
            return false;
        }
        texto += entradaDic;

        // Agregamos al diccionario la secuencia anterior mas el primer byte de
        // la secuencia actual. El compresor hizo esta misma insercion cuando
        // encontro la secuencia nueva.
        if (!anterior.empty() && siguiente < MAX_CODIGOS) {
            std::string nueva = anterior;
            nueva.push_back(entradaDic[0]);
            palabras[siguiente] = nueva;
            siguiente++;
        }
        anterior = entradaDic;
    }

    delete[] palabras;

    // Al final comprobamos que hayamos recuperado exactamente los bytes que
    // decia el encabezado. Si falta o sobra algo, el archivo esta truncado.
    if (texto.size() != static_cast<size_t>(original)) {
        error = "el texto descomprimido no coincide con el tamano original";
        return false;
    }
    return true;
}

}  // namespace lzw
