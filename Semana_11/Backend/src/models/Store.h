#pragma once
// Store: los usuarios, las solicitudes y el grafo, todo SOLO en memoria
// (al apagar el servidor se pierde, aqui no hay data.json ni nada parecido).
// Es singleton con mutex porque el servidor atiende en varios hilos y no
// queremos que dos peticiones toquen la lista al mismo tiempo.

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "Grafo.h"

// Un usuario es solo nombre + correo + password ya hasheada (nunca en claro).
struct Usuario {
    std::string nombre;
    std::string correo;
    std::string passwordHash;
};

// Una solicitud dice quien la mando (de), a quien (para) y en que va:
// pendiente, aceptada o rechazada. Solo al aceptarla nace la arista.
struct Solicitud {
    std::string de;
    std::string para;
    std::string estado;  // pendiente | aceptada | rechazada
};

class Store {
public:
    static Store& get();

    void reset();  // vacia todo (endpoint /api/reset, util para probar en el lab)

    std::map<std::string, Usuario> usuarios;
    std::vector<Solicitud> solicitudes;
    GrafoNoDirigido grafo;
    std::mutex mtx;

private:
    Store() = default;
};
