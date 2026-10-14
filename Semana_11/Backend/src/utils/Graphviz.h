#pragma once
// Convierte el DOT en PNG llamando al programa `dot` de Graphviz.
// Ojo: Graphviz aqui SOLO dibuja. El grafo (vertices, aristas, colores)
// se construye a mano en models/Grafo, el `dot` unicamente lo renderiza.

#include <string>

namespace graphviz {

// Dice si el binario `dot` esta en el PATH.
bool disponible();

// Le pasa el texto DOT al `dot` y devuelve los bytes del PNG.
// Si algo falla devuelve "" y el motivo queda en ultimoError().
std::string renderDotAPng(const std::string& dot);
std::string ultimoError();

}  // namespace graphviz
