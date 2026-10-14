#include "Store.h"

// El singleton: la primera vez que se pide se crea, y de ahi en mas todos
// los controladores usan esa misma instancia (o sea, los mismos datos).
Store& Store::get() {
    static Store inst;
    return inst;
}

// Borra usuarios, solicitudes y deja un grafo nuevecito.
void Store::reset() {
    usuarios.clear();
    solicitudes.clear();
    grafo = GrafoNoDirigido();
}
