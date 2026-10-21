#include "Store.h"

// El singleton: la primera vez que se pide se crea, y de ahi en mas todos
// los controladores usan esa misma instancia (o sea, los mismos datos).
Store& Store::get() {
    static Store inst;
    return inst;
}

// Busca el correo recorriendo la lista (sin map, a puro ciclo).
int Store::buscarUsuario(const std::string& correo) const {
    for (size_t i = 0; i < usuarios.tam(); i++)
        if (usuarios[i].correo == correo) return (int)i;
    return -1;
}

// Borra usuarios, solicitudes y deja un grafo nuevecito.
void Store::reset() {
    usuarios.limpiar();
    solicitudes.limpiar();
    grafo = GrafoNoDirigido();
}
