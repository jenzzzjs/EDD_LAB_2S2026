#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

// La compresion recorre el mensaje y arma secuencias. Si una secuencia ya
// existe, escribe su codigo; si no, la agrega al diccionario. La descompresion
// reconstruye el diccionario en el mismo orden para recuperar el mensaje.

// El programa usa LZW con codigos de 12 bits. Los codigos 0 a 255 representan
// bytes, el 256 es CLEAR y los demas se agregan al diccionario.
const int MAX_CODIGOS = 4096;

// El codigo 256 se reserva para reiniciar el diccionario.
const int CODIGO_CLEAR = 256;

// Ponemos un limite al mensaje para no consumir memoria sin control.
const std::size_t MAX_MENSAJE = 1024u * 1024u;

// Limite del archivo comprimido que se puede leer.
const std::size_t MAX_ARCHIVO = 4u * 1024u * 1024u;

// El archivo comprimido siempre se guarda con este nombre.
const char* ARCHIVO_LZW = "mensaje.lzw";

// Esta marca identifica el formato del texto comprimido.
const char* MAGIA = "LZW1";

// Los codigos ocupan 12 bits, pero el texto comprimido se escribe en bytes de
// 8 bits. Acumulamos bits y escribimos un byte cada vez que completamos ocho.
class EscritorBits {
public:
    // Guardamos una referencia al texto donde se agregan los bytes.
    explicit EscritorBits(std::string& destino) : destino_(destino) {}

    void escribir(std::uint16_t codigo) {
        // Metemos los 12 bits del codigo a partir de la posicion libre.
        buffer_ |= static_cast<std::uint32_t>(codigo) << bits_;

        // Sumamos los 12 bits nuevos a la cantidad de bits pendientes.
        bits_ += 12;

        // Mientras haya ocho bits o mas, sacamos un byte completo.
        while (bits_ >= 8) {
            // Guardamos el byte menos significativo del buffer.
            destino_.push_back(static_cast<char>(buffer_ & 0xFF));

            // Descartamos ese byte y dejamos los bits restantes.
            buffer_ >>= 8;

            // Actualizamos cuantos bits quedan pendientes.
            bits_ -= 8;
        }
    }

    void cerrar() {
        // Si quedaron bits sin completar un byte, los enviamos con ceros.
        if (bits_ > 0) {
            // Escribimos el ultimo byte aunque no este completo.
            destino_.push_back(static_cast<char>(buffer_ & 0xFF));
        }
    }

private:
    std::string& destino_;       // Texto donde se guardan los bytes.
    std::uint32_t buffer_ = 0;   // Bits acumulados.
    int bits_ = 0;               // Cantidad de bits validos en el buffer.
};

// Leemos bytes hasta tener 12 bits. Si al final no alcanzan, ya no hay mas
// codigos y el ciclo termina.
class LectorBits {
public:
    // Guardamos el texto comprimido y la posicion del primer codigo.
    LectorBits(const std::string& origen, std::size_t inicio)
        : origen_(origen), pos_(inicio) {}

    bool leer(std::uint16_t& codigo) {
        // Cargamos bytes hasta tener los 12 bits de un codigo.
        while (bits_ < 12 && pos_ < origen_.size()) {
            // Tomamos el byte actual como valor sin signo.
            buffer_ |= static_cast<std::uint32_t>(
                           static_cast<unsigned char>(origen_[pos_]))
                       << bits_;

            // Sumamos los ocho bits que acabamos de leer.
            bits_ += 8;

            // Avanzamos a la posicion del siguiente byte.
            pos_++;
        }

        // Si no se completaron 12 bits, ya no queda un codigo entero.
        if (bits_ < 12) return false;

        // Tomamos solamente los 12 bits bajos, que forman el codigo.
        codigo = static_cast<std::uint16_t>(buffer_ & 0x0FFF);

        // Descartamos del buffer los 12 bits que ya usamos.
        buffer_ >>= 12;

        // Actualizamos la cantidad de bits que siguen pendientes.
        bits_ -= 12;

        // Avisamos que el codigo se leyo correctamente.
        return true;
    }

private:
    const std::string& origen_;  // Texto comprimido del que salen los bytes.
    std::size_t pos_;            // Posicion del siguiente byte por leer.
    std::uint32_t buffer_ = 0;   // Bits que todavia no forman un codigo.
    int bits_ = 0;               // Cantidad de bits validos en el buffer.
};

// Guardamos el tamano original en 8 bytes. El descompresor lo usa para validar
// que el resultado final sea exactamente el mensaje que se comprimio.
void escribir64(std::string& salida, std::uint64_t valor) {
    // Recorremos los ocho bytes que forman el numero.
    for (int i = 0; i < 8; i++) {
        // Extraemos el byte menos significativo y lo pasamos al texto.
        salida.push_back(static_cast<char>((valor >> (i * 8)) & 0xFF));
    }
}

std::uint64_t leer64(const std::string& datos, std::size_t pos) {
    // Aqui armamos el numero byte por byte.
    std::uint64_t valor = 0;

    // Leemos los ocho bytes en el mismo orden en que se escribieron.
    for (int i = 0; i < 8; i++) {
        // Colocamos cada byte en su posicion dentro del numero.
        valor |= static_cast<std::uint64_t>(
                     static_cast<unsigned char>(datos[pos + i]))
                 << (i * 8);
    }

    // Devolvemos el numero completo.
    return valor;
}

bool comprimir(const std::string& mensaje, std::string& salida, std::string& error) {
    // Borramos cualquier resultado o error de una llamada anterior.
    error.clear();
    salida.clear();

    // Un mensaje vacio no tiene nada que comprimir.
    if (mensaje.empty()) {
        error = "El mensaje esta vacio.";
        return false;
    }

    // Evitamos que un mensaje enorme consuma toda la memoria.
    if (mensaje.size() > MAX_MENSAJE) {
        error = "El mensaje supera el limite de 1 MB.";
        return false;
    }

    // El encabezado guarda la identificacion y el tamano del texto original.
    salida.append(MAGIA, 4);
    escribir64(salida, static_cast<std::uint64_t>(mensaje.size()));

    // El diccionario empieza con los 256 bytes literales. El codigo 256 esta
    // reservado para CLEAR, por eso el primer codigo nuevo es el 257.
    std::unordered_map<std::string, int> diccionario;

    // Recorremos los 256 valores posibles de un byte.
    for (int i = 0; i < 256; i++) {
        // La clave es el byte y el valor es su codigo.
        diccionario[std::string(1, static_cast<char>(i))] = i;
    }

    // Guardamos el siguiente codigo libre del diccionario.
    int siguiente = CODIGO_CLEAR + 1;

    // Este objeto empaqueta los codigos de 12 bits en bytes.
    EscritorBits escritor(salida);

    // Guardamos la secuencia que estamos probando en cada vuelta.
    std::string actual;

    // Recorremos el mensaje byte por byte y mantenemos la secuencia mas larga
    // que ya existe en el diccionario.
    for (char c : mensaje) {
        // Unimos la secuencia actual con el byte nuevo.
        std::string candidato = actual + c;

        // Si la secuencia existe, la alargamos y seguimos leyendo.
        if (diccionario.find(candidato) != diccionario.end()) {
            // La secuencia nueva ya estaba en el diccionario.
            actual = candidato;

            // Pasamos al siguiente byte sin escribir nada.
            continue;
        }

        // Si no existe, escribimos el codigo de la secuencia actual. Esa
        // secuencia ya estaba en el diccionario.
        escritor.escribir(static_cast<std::uint16_t>(diccionario.at(actual)));

        // Agregamos la secuencia nueva si todavia hay espacio.
        if (siguiente < MAX_CODIGOS) {
            // Guardamos la secuencia nueva con el siguiente codigo libre.
            diccionario[candidato] = siguiente;

            // Avanzamos al siguiente codigo disponible.
            siguiente++;
        }

        // Cuando el diccionario se llena mandamos CLEAR y lo reiniciamos para
        // que el descompresor haga exactamente lo mismo.
        if (siguiente >= MAX_CODIGOS) {
            // Avisamos al descompresor que debe borrar el diccionario.
            escritor.escribir(CODIGO_CLEAR);

            // Borramos todas las secuencias que se habian agregado.
            diccionario.clear();

            // Volvemos a cargar los 256 bytes literales.
            for (int i = 0; i < 256; i++) {
                diccionario[std::string(1, static_cast<char>(i))] = i;
            }

            // El siguiente codigo nuevo vuelve a ser 257.
            siguiente = CODIGO_CLEAR + 1;
        }

        // Empezamos una secuencia nueva con el byte que no coincidio.
        actual.assign(1, c);
    }

    // Al terminar el recorrido todavia queda una secuencia por escribir.
    if (!actual.empty()) {
        // Escribimos el codigo de la ultima secuencia.
        escritor.escribir(static_cast<std::uint16_t>(diccionario.at(actual)));
    }

    // Completamos el ultimo byte si quedaron bits pendientes.
    escritor.cerrar();

    // Avisamos que la compresion termino correctamente.
    return true;
}

bool descomprimir(const std::string& entrada, std::string& mensaje, std::string& error) {
    // Borramos cualquier resultado o error de una llamada anterior.
    error.clear();
    mensaje.clear();

    // Antes de leer codigos validamos el encabezado y el tamano original.
    if (entrada.size() < 12 || entrada.compare(0, 4, MAGIA) != 0) {
        error = "El texto comprimido no tiene el formato LZW esperado.";
        return false;
    }

    // Leemos el tamano que tenia el mensaje antes de comprimirlo.
    std::uint64_t original = leer64(entrada, 4);

    // Si el tamano es demasiado grande, no reservamos memoria.
    if (original > MAX_MENSAJE) {
        error = "El tamano original supera el limite de 1 MB.";
        return false;
    }

    // Reservamos el espacio que el compresor dijo que necesitabamos.
    mensaje.reserve(static_cast<std::size_t>(original));

    // Reconstruimos el diccionario inicial con los 256 bytes literales.
    std::vector<std::string> diccionario(MAX_CODIGOS);

    // Recorremos los 256 valores posibles de un byte.
    for (int i = 0; i < 256; i++) {
        // Cada codigo inicial representa un solo byte.
        diccionario[i] = std::string(1, static_cast<char>(i));
    }

    // Guardamos el siguiente codigo libre del diccionario.
    int siguiente = CODIGO_CLEAR + 1;

    // Guardamos la secuencia que se decodifico en la vuelta anterior.
    std::string anterior;

    // El lector empieza despues del encabezado de 12 bytes.
    LectorBits lector(entrada, 12);

    // Aqui se guarda cada codigo que se lee.
    std::uint16_t codigo;

    // Leemos codigos de 12 bits hasta que se acaben los bytes utiles.
    while (lector.leer(codigo)) {
        // CLEAR indica que el compresor reinicio el diccionario.
        if (codigo == CODIGO_CLEAR) {
            // Reiniciamos el siguiente codigo libre.
            siguiente = CODIGO_CLEAR + 1;

            // Olvidamos la secuencia anterior.
            anterior.clear();

            // Saltamos al siguiente codigo.
            continue;
        }

        // Aqui guardaremos la secuencia que representa el codigo.
        std::string secuencia;

        // Si el codigo ya existe, copiamos su secuencia del diccionario.
        if (codigo < siguiente && codigo < MAX_CODIGOS) {
            // Recuperamos la secuencia guardada en esa posicion.
            secuencia = diccionario[codigo];
        } else if (codigo == siguiente && !anterior.empty()) {
            // Caso especial de LZW: el codigo todavia no esta en el
            // diccionario. La secuencia es anterior mas su primer byte.
            secuencia = anterior;

            // Agregamos el primer byte de la secuencia anterior.
            secuencia.push_back(anterior[0]);
        } else {
            // Si no cumple ninguna regla, el codigo no es valido.
            error = "Se encontro un codigo LZW invalido.";
            return false;
        }

        // Una secuencia valida nunca puede quedar vacia. Si eso pasa, el
        // codigo no corresponde a ninguna entrada del diccionario.
        if (secuencia.empty()) {
            error = "Se encontro una secuencia LZW vacia.";
            return false;
        }

        // Si el texto crece mas de lo que decia el encabezado, el texto
        // comprimido esta danado o no corresponde a ese tamano.
        if (mensaje.size() + secuencia.size() > original) {
            error = "El texto descomprimido supera el tamano esperado.";
            return false;
        }

        // Agregamos la secuencia al mensaje recuperado.
        mensaje += secuencia;

        // Agregamos al diccionario la secuencia anterior mas el primer byte de
        // la secuencia actual. El compresor hizo esta misma insercion.
        if (!anterior.empty() && siguiente < MAX_CODIGOS) {
            // Guardamos la secuencia combinada en el siguiente codigo libre.
            diccionario[siguiente] = anterior + secuencia[0];

            // Avanzamos al siguiente codigo disponible.
            siguiente++;
        }

        // Guardamos la secuencia actual para usarla en la proxima vuelta.
        anterior = secuencia;
    }

    // Al final el tamano debe coincidir exactamente con el original.
    if (mensaje.size() != static_cast<std::size_t>(original)) {
        error = "El texto descomprimido no coincide con el tamano original.";
        return false;
    }

    // Avisamos que la descompresion termino correctamente.
    return true;
}

bool guardarArchivo(const std::string& ruta, const std::string& datos, std::string& error) {
    // Abrimos en modo binario y reemplazamos el contenido anterior.
    std::ofstream archivo(ruta, std::ios::binary | std::ios::trunc);

    // Si no se pudo abrir, avisamos que no se puede guardar.
    if (!archivo.is_open()) {
        error = "No se pudo crear el archivo " + ruta + ".";
        return false;
    }

    // Escribimos todos los bytes comprimidos de una sola vez.
    archivo.write(datos.data(), static_cast<std::streamsize>(datos.size()));

    // Si la escritura fallo, avisamos el problema.
    if (!archivo) {
        error = "No se pudo escribir el archivo " + ruta + ".";
        return false;
    }

    // Cerramos el archivo para confirmar que se guardo en el disco.
    archivo.close();

    // Si el cierre fallo, el archivo puede haber quedado incompleto.
    if (!archivo) {
        error = "No se pudo cerrar el archivo " + ruta + ".";
        return false;
    }

    return true;
}

bool leerArchivo(const std::string& ruta, std::string& datos, std::string& error) {
    // Borramos cualquier contenido anterior.
    datos.clear();

    // Abrimos el archivo en modo binario para no alterar ningun byte.
    std::ifstream archivo(ruta, std::ios::binary);

    // Si el archivo no existe, pedimos que se revise la ruta indicada.
    if (!archivo.is_open()) {
        error = "No se pudo abrir el archivo " + ruta + ". Verifica la ruta.";
        return false;
    }

    // Movemos el cursor al final para medir el archivo.
    archivo.seekg(0, std::ios::end);
    std::streamoff tam = archivo.tellg();

    // Si no se pudo medir, no seguimos.
    if (tam < 0) {
        error = "No se pudo medir el archivo " + ruta + ".";
        return false;
    }

    // Evitamos cargar un archivo demasiado grande.
    if (static_cast<std::uint64_t>(tam) > MAX_ARCHIVO) {
        error = "El archivo " + ruta + " es demasiado grande.";
        return false;
    }

    // Volvemos al inicio para leer desde el primer byte.
    archivo.seekg(0, std::ios::beg);

    // Si no se pudo volver al inicio, avisamos el error.
    if (!archivo) {
        error = "No se pudo volver al inicio del archivo " + ruta + ".";
        return false;
    }

    // Reservamos exactamente el tamano que se midio.
    datos.resize(static_cast<std::size_t>(tam));

    // Si el archivo tiene contenido, lo leemos completo.
    if (tam > 0) {
        archivo.read(&datos[0], static_cast<std::streamsize>(tam));

        // Si no se leyeron todos los bytes, el archivo esta incompleto.
        if (!archivo) {
            error = "No se pudo leer completo el archivo " + ruta + ".";
            return false;
        }
    }

    return true;
}

void mostrarMenu() {
    // Mostramos las tres opciones disponibles.
    std::cout << "\n=== LZW de mensajes ===\n";
    std::cout << "1. Comprimir \n";
    std::cout << "2. Descomprimir \n";
    std::cout << "3. Salir\n";
    std::cout << "Opcion: ";
}

void comprimirMensaje() {
    // Pedimos el mensaje y lo leemos como una linea completa.
    std::cout << "Escribe el mensaje: ";
    std::string mensaje;
    std::getline(std::cin, mensaje);

    // Aqui guardamos el resultado comprimido y el mensaje de error.
    std::string comprimido;
    std::string error;

    // Si la compresion falla, mostramos el motivo.
    if (!comprimir(mensaje, comprimido, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    // Guardamos el resultado en el archivo de nombre fijo.
    if (!guardarArchivo(ARCHIVO_LZW, comprimido, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    // Mostramos los tamanos y el nombre del archivo generado.
    std::cout << "Original: " << mensaje.size() << " bytes\n";
    std::cout << "Comprimido: " << comprimido.size() << " bytes\n";
    std::cout << "Archivo guardado: " << ARCHIVO_LZW << "\n";

    // Leemos el archivo de nuevo para comprobar que se guardo completo.
    std::string guardado;
    if (!leerArchivo(ARCHIVO_LZW, guardado, error)) {
        std::cout << "Error de verificacion: " << error << "\n";
        return;
    }

    // Descomprimimos lo leido para compararlo con el mensaje original.
    std::string recuperado;
    if (!descomprimir(guardado, recuperado, error)) {
        std::cout << "Error de verificacion: " << error << "\n";
    } else if (recuperado == mensaje) {
        // Comparamos el texto recuperado con el mensaje original.
        std::cout << "Verificacion: el mensaje se recupero correctamente.\n";
    } else {
        // Si son distintos, la verificacion no paso.
        std::cout << "Verificacion: el mensaje recuperado es distinto.\n";
    }
}

void descomprimirMensaje() {
    // Pedimos la ruta del archivo que se quiere descomprimir.
    std::cout << "Ruta del archivo a descomprimir: ";
    std::string ruta;
    std::getline(std::cin, ruta);

    // Quitamos las comillas si se pego una ruta copiada de Windows.
    if (ruta.size() >= 2 && ruta.front() == '"' && ruta.back() == '"') {
        ruta = ruta.substr(1, ruta.size() - 2);
    }

    // Una ruta vacia no permite abrir ningun archivo.
    if (ruta.empty()) {
        std::cout << "Error: la ruta esta vacia.\n";
        return;
    }

    // Aqui guardamos los bytes del archivo y el mensaje de error.
    std::string comprimido;
    std::string error;

    // Leemos el archivo indicado por el usuario.
    if (!leerArchivo(ruta, comprimido, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    // Aqui guardamos el mensaje recuperado.
    std::string mensaje;

    // Si el texto comprimido no es valido, mostramos el error.
    if (!descomprimir(comprimido, mensaje, error)) {
        std::cout << "Error: " << error << "\n";
        return;
    }

    // Mostramos el mensaje que se recupero.
    std::cout << "Mensaje: " << mensaje << "\n";
}

}  // namespace

int main() {
    // Guardamos la opcion que escribe el usuario.
    std::string opcion;

    // Repetimos el menu hasta que el usuario elija salir o se termine la
    // entrada.
    while (true) {
        // Mostramos el menu y leemos la opcion.
        mostrarMenu();
        if (!std::getline(std::cin, opcion)) break;

        if (opcion == "1") {
            // La opcion 1 comprime y guarda mensaje.lzw.
            comprimirMensaje();
        } else if (opcion == "2") {
            // La opcion 2 pide una ruta y descomprime ese archivo.
            descomprimirMensaje();
        } else if (opcion == "3") {
            // La opcion 3 termina el programa.
            break;
        } else {
            // Cualquier otra opcion no es valida.
            std::cout << "Opcion invalida.\n";
        }
    }

    // Mostramos un mensaje final antes de cerrar.
    std::cout << "gg.\n";
    return 0;
}
