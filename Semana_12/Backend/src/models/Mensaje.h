#pragma once
// Mensaje representa un mensaje ya guardado. El campo `de` guarda el correo
// de quien lo manda, el campo `para` guarda el correo del amigo, `texto`
// guarda el contenido y `hora` guarda los milisegundos desde epoch como
// texto.
//
// LzwInfo no es parte del mensaje. Es la informacion que el backend devuelve
// para que la interfaz pueda mostrar en que archivo se guardo la conversacion,
// cuantos bytes tenia el texto original, cuantos ocupa el archivo comprimido y
// que porcentaje se ahorro. El chat se guarda por pareja de amigos, por eso una
// conversacion completa vive en un solo archivo LZW.

#include <cstddef>
#include <string>

struct Mensaje {
    std::string id;
    std::string de;
    std::string para;
    std::string texto;
    std::string hora;
};

struct LzwInfo {
    std::string archivo;
    size_t bytesOriginal = 0;
    size_t bytesComprimido = 0;
};
