#pragma once
// Cada pareja de amigos tiene un solo archivo dentro de la carpeta lzw. El
// historial completo se convierte en texto, con un mensaje JSON por linea, y
// despues se comprime con LZW. Al abrir la conversacion se lee el archivo, se
// descomprime y se reconstruye la lista de mensajes.
//
// Los mensajes tambien se mantienen en memoria para no leer y descomprimir el
// archivo en cada peticion. El archivo es la fuente de verdad cuando el
// servidor arranca de nuevo, porque la memoria se pierde al cerrar el proceso.
//
// El formato guarda un solo bloque LZW por conversacion. Cuando llega un
// mensaje nuevo, se vuelve a serializar toda la conversacion y se recompone el
// archivo completo. Es mas simple que guardar bloques separados y permite que
// el diccionario LZW aproveche las palabras repetidas de todos los mensajes.

#include <mutex>
#include <string>

#include "Lista.h"
#include "Mensaje.h"

class ChatStore {
public:
    struct ResultadoHistorial {
        bool ok = false;
        std::string error;
        Lista<Mensaje> mensajes;
        LzwInfo lzw;
    };

    struct ResultadoEnvio {
        bool ok = false;
        std::string error;
        Mensaje mensaje;
        LzwInfo lzw;
    };

    static ChatStore& get();

    // Devuelve el historial entre dos correos, sin importar quien inicio la
    // conversacion. El controlador se encarga de validar que sean amigos.
    ResultadoHistorial historial(const std::string& a, const std::string& b);

    // Agrega un mensaje, vuelve a serializar la conversacion completa, la
    // comprime con LZW y escribe el archivo.
    ResultadoEnvio enviar(const std::string& de, const std::string& para,
                          const std::string& texto);

    std::string carpeta() const;

private:
    ChatStore();

    // Carga una conversacion desde disco una sola vez. Despues de cargarla,
    // los mensajes quedan en memoria y no se vuelve a leer el mismo archivo.
    bool cargar(const std::string& clave, const std::string& ruta, std::string& error);

    std::mutex mtx_;
    Lista<Mensaje> mensajes_;
    Lista<Par> cargadas_;
};
