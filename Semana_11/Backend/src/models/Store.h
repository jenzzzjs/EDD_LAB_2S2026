#pragma once
// Store: los usuarios, las solicitudes y el grafo, todo SOLO en memoria
// (al apagar el servidor se pierde, aqui no hay data.json ni nada parecido).
// Sin std::vector ni std::map: usuarios y solicitudes viven en la Lista
// enlazada hecha a mano y se buscan con ciclos. Es singleton con mutex
// porque el servidor atiende en varios hilos y no queremos que dos
// peticiones toquen la lista al mismo tiempo.

#include <mutex>
#include <string>

#include "Grafo.h"
#include "Lista.h"

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

    // Posicion del usuario por correo, o -1 si no existe (busqueda lineal).
    int buscarUsuario(const std::string& correo) const;

    Lista<Usuario> usuarios;
    Lista<Solicitud> solicitudes;
    GrafoNoDirigido grafo;
    std::mutex mtx;

private:
    Store() = default;
};
