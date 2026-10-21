#include "ChatStore.h"

#include <atomic>
#include <cctype>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <system_error>

#include "../utils/Json.h"
#include "../utils/Lzw.h"
#include "../utils/Sha256.h"

namespace {

// Cada conversacion tiene un archivo propio. Para que dos usuarios siempre
// lleguen al mismo archivo, ordenamos los correos antes de construir la clave.
// El nombre visible usa los correos saneados y un hash corto para evitar
// caracteres raros y colisiones.

std::string minusculas(const std::string& s) {
    std::string out = s;
    for (size_t i = 0; i < out.size(); i++)
        out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
    return out;
}

std::string recortar(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) a++;
    size_t b = s.size();
    while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\r' || s[b - 1] == '\n')) b--;
    return s.substr(a, b - a);
}

// La clave de carga ordena los dos correos. Asi la pareja ana y bruno produce
// la misma clave que bruno y ana, y solo se carga una vez.
std::string claveCarga(const std::string& a, const std::string& b) {
    std::string x = minusculas(recortar(a));
    std::string y = minusculas(recortar(b));
    if (y < x) {
        std::string t = x;
        x = y;
        y = t;
    }
    return x + "\x1F" + y;
}

// El correo puede tener arrobas, puntos y otros caracteres que no conviene
// usar directamente en un nombre de archivo. Los reemplazamos por guiones o
// por el texto _at_. El nombre final nunca sale de la carpeta lzw.
std::string sanitizar(const std::string& correo) {
    std::string out;
    for (size_t i = 0; i < correo.size(); i++) {
        unsigned char c = static_cast<unsigned char>(correo[i]);
        if (std::isalnum(c)) {
            out.push_back(static_cast<char>(std::tolower(c)));
        } else if (c == '.' || c == '-' || c == '_') {
            out.push_back(static_cast<char>(c));
        } else if (c == '@') {
            out += "_at_";
        } else {
            out.push_back('_');
        }
    }
    if (out.size() > 34) out = out.substr(0, 34);
    if (out.empty()) out = "usuario";
    return out;
}

// Construimos la ruta del archivo LZW de la pareja. Los correos se ordenan para
// que el nombre sea el mismo sin importar quien escribe primero. El hash corto
// se calcula con SHA-256 y sirve para diferenciar parejas que, despues de
// sanear los correos, podrian quedar con un nombre parecido.
std::string rutaArchivo(const std::string& a, const std::string& b) {
    std::string x = minusculas(recortar(a));
    std::string y = minusculas(recortar(b));
    if (y < x) {
        std::string t = x;
        x = y;
        y = t;
    }
    std::string clave = x + "|" + y;
    std::string hash = sha256::hashHex(clave).substr(0, 10);
    std::string nombre = "chat_" + sanitizar(x) + "__" + sanitizar(y) + "_" + hash + ".lzw";
    return (std::filesystem::path("lzw") / nombre).generic_string();
}

// Antes de comprimir convertimos la lista de mensajes en texto. Cada mensaje
// ocupa una linea JSON y termina con un salto de linea. Este texto es el que
// LZW va a comprimir y el que se vuelve a leer cuando se descomprime.
std::string serializar(const Lista<Mensaje>& mensajes) {
    std::string salida;
    for (size_t i = 0; i < mensajes.tam(); i++) {
        const Mensaje& m = mensajes[i];
        salida += "{\"id\":" + json::str(m.id) + ",\"de\":" + json::str(m.de) +
                  ",\"para\":" + json::str(m.para) + ",\"texto\":" + json::str(m.texto) +
                  ",\"hora\":" + json::str(m.hora) + "}\n";
    }
    return salida;
}

// Despues de descomprimir volvemos a leer el texto linea por linea. Cada linea
// debe empezar con una llave porque representa un objeto JSON. Si encontramos
// una linea que no cumple, el archivo LZW esta danado y devolvemos un error.
bool parsear(const std::string& texto, Lista<Mensaje>& salida, std::string& error) {
    size_t inicio = 0;
    while (inicio < texto.size()) {
        size_t fin = texto.find('\n', inicio);
        if (fin == std::string::npos) fin = texto.size();
        std::string linea = recortar(texto.substr(inicio, fin - inicio));
        inicio = fin + 1;
        if (linea.empty()) continue;
        if (linea[0] != '{') {
            error = "linea invalida en el archivo LZW";
            return false;
        }
        Mensaje m;
        m.id = json::getString(linea, "id");
        m.de = json::getString(linea, "de");
        m.para = json::getString(linea, "para");
        m.texto = json::getString(linea, "texto");
        m.hora = json::getString(linea, "hora");
        if (m.de.empty() || m.para.empty()) {
            error = "mensaje invalido en el archivo LZW";
            return false;
        }
        salida.push_back(m);
    }
    return true;
}

// Leemos el archivo completo en modo binario. Adentro no hay texto plano sino
// bytes LZW, por eso no podemos tratarlo como una cadena normal.
bool leerArchivo(const std::string& ruta, std::string& contenido, std::string& error) {
    std::ifstream archivo(ruta, std::ios::binary);
    if (!archivo) {
        error = "no se pudo abrir " + ruta;
        return false;
    }
    std::ostringstream ss;
    ss << archivo.rdbuf();
    contenido = ss.str();
    return true;
}

// Escribimos el bloque LZW completo en modo binario y truncamos el archivo
// anterior. Como cada mensaje nuevo recompone toda la conversacion, no
// necesitamos agregar bytes al final: reemplazamos el archivo entero.
bool escribirArchivo(const std::string& ruta, const std::string& contenido, std::string& error) {
    std::error_code ec;
    std::filesystem::create_directories(std::filesystem::path(ruta).parent_path(), ec);
    std::ofstream archivo(ruta, std::ios::binary | std::ios::trunc);
    if (!archivo) {
        error = "no se pudo crear " + ruta;
        return false;
    }
    archivo.write(contenido.data(), static_cast<std::streamsize>(contenido.size()));
    if (!archivo.good()) {
        error = "no se pudo escribir " + ruta;
        return false;
    }
    return true;
}

std::string ahoraMs() {
    using namespace std::chrono;
    auto ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    return std::to_string(static_cast<long long>(ms));
}

std::string nuevoId() {
    static std::atomic<unsigned long long> contador{0};
    return ahoraMs() + "-" + std::to_string(++contador);
}

}  // namespace

ChatStore& ChatStore::get() {
    static ChatStore instancia;
    return instancia;
}

ChatStore::ChatStore() {
    // Nos aseguramos de que la carpeta lzw exista antes de intentar leer o
    // escribir archivos comprimidos.
    std::error_code ec;
    std::filesystem::create_directories("lzw", ec);
}

std::string ChatStore::carpeta() const {
    return std::filesystem::path("lzw").generic_string();
}

// Cargamos una conversacion desde su archivo LZW. Si la pareja ya se cargo
// antes, no volvemos a leer el disco. Si el archivo existe, lo leemos, lo
// descomprimimos y convertimos el texto en mensajes. Si no existe, la
// conversacion empieza vacia y el archivo se creara con el primer mensaje.
bool ChatStore::cargar(const std::string& clave, const std::string& ruta, std::string& error) {
    for (size_t i = 0; i < cargadas_.tam(); i++) {
        if (cargadas_[i].clave == clave) return true;
    }

    Lista<Mensaje> nuevos;
    std::error_code ec;
    bool existe = std::filesystem::exists(ruta, ec);
    if (ec) {
        error = "no se pudo consultar " + ruta + ": " + ec.message();
        return false;
    }
    if (existe) {
        std::string comprimido;
        if (!leerArchivo(ruta, comprimido, error)) return false;

        // Aqui ocurre la descompresion LZW. El archivo guarda un encabezado
        // con el tamano original y despues los codigos de 12 bits. El modulo
        // Lzw se encarga de reconstruir el texto completo.
        std::string texto;
        std::string errorLzw;
        if (!lzw::descomprimir(comprimido, texto, errorLzw)) {
            error = "no se pudo descomprimir " + ruta + ": " + errorLzw;
            return false;
        }
        if (!parsear(texto, nuevos, error)) return false;
    }

    // Pasamos los mensajes nuevos a la lista general y marcamos la pareja como
    // cargada para no repetir el trabajo.
    for (size_t i = 0; i < nuevos.tam(); i++) mensajes_.push_back(nuevos[i]);
    cargadas_.push_back(Par{clave, "1"});
    return true;
}

// Devolvemos solo los mensajes de la pareja consultada. Primero nos aseguramos
// de que la conversacion este cargada, lo que puede implicar descomprimir el
// archivo LZW. Despues filtramos la lista en memoria y calculamos los datos de
// compresion que la interfaz muestra en la tarjeta de LZW.
ChatStore::ResultadoHistorial ChatStore::historial(const std::string& a,
                                                   const std::string& b) {
    ResultadoHistorial resultado;
    std::lock_guard<std::mutex> lock(mtx_);

    std::string ca = minusculas(recortar(a));
    std::string cb = minusculas(recortar(b));
    std::string clave = claveCarga(ca, cb);
    std::string ruta = rutaArchivo(ca, cb);

    if (!cargar(clave, ruta, resultado.error)) return resultado;

    for (size_t i = 0; i < mensajes_.tam(); i++) {
        const Mensaje& m = mensajes_[i];
        bool deAparaB = (m.de == ca && m.para == cb);
        bool deBparaA = (m.de == cb && m.para == ca);
        if (deAparaB || deBparaA) resultado.mensajes.push_back(m);
    }

    resultado.ok = true;
    if (!resultado.mensajes.vacia()) {
        // El tamano original es el texto JSON antes de comprimir. El tamano
        // comprimido sale del archivo .lzw que esta en disco.
        std::string texto = serializar(resultado.mensajes);
        resultado.lzw.archivo = ruta;
        resultado.lzw.bytesOriginal = texto.size();
        std::error_code ec;
        auto tam = std::filesystem::file_size(ruta, ec);
        if (!ec) resultado.lzw.bytesComprimido = static_cast<size_t>(tam);
    }
    return resultado;
}

// Guardamos un mensaje nuevo. Primero cargamos la conversacion, despues
// agregamos el mensaje a la lista en memoria, serializamos toda la pareja y
// comprimimos el texto completo con LZW. El archivo anterior se reemplaza por
// el nuevo bloque comprimido.
ChatStore::ResultadoEnvio ChatStore::enviar(const std::string& de, const std::string& para,
                                            const std::string& texto) {
    ResultadoEnvio resultado;
    std::lock_guard<std::mutex> lock(mtx_);

    std::string cDe = minusculas(recortar(de));
    std::string cPara = minusculas(recortar(para));
    std::string clave = claveCarga(cDe, cPara);
    std::string ruta = rutaArchivo(cDe, cPara);

    if (!cargar(clave, ruta, resultado.error)) return resultado;

    // Reunimos solo los mensajes de esta pareja. La lista general puede tener
    // conversaciones de otros usuarios, pero no deben entrar en este archivo.
    Lista<Mensaje> conversacion;
    for (size_t i = 0; i < mensajes_.tam(); i++) {
        const Mensaje& m = mensajes_[i];
        if ((m.de == cDe && m.para == cPara) || (m.de == cPara && m.para == cDe))
            conversacion.push_back(m);
    }

    Mensaje mensaje;
    mensaje.id = nuevoId();
    mensaje.de = cDe;
    mensaje.para = cPara;
    mensaje.texto = texto;
    mensaje.hora = ahoraMs();
    conversacion.push_back(mensaje);

    // Serializamos la conversacion completa y la comprimimos. El diccionario
    // LZW se arma de nuevo en cada escritura, asi que el archivo siempre es
    // autocontenido y se puede descomprimir sin depender de escrituras previas.
    std::string plano = serializar(conversacion);
    std::string comprimido;
    std::string errorLzw;
    if (!lzw::comprimir(plano, comprimido, errorLzw)) {
        resultado.error = "no se pudo comprimir el chat: " + errorLzw;
        return resultado;
    }
    if (!escribirArchivo(ruta, comprimido, resultado.error)) return resultado;

    mensajes_.push_back(mensaje);
    resultado.ok = true;
    resultado.mensaje = mensaje;
    resultado.lzw.archivo = ruta;
    resultado.lzw.bytesOriginal = plano.size();
    resultado.lzw.bytesComprimido = comprimido.size();
    return resultado;
}
