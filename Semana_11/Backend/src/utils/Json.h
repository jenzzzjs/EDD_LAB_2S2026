// Utilidades JSON minimas, hechas a mano (sin librerias externas).
// Solo lo necesario para el API: escapar strings, leer un campo string
// de un objeto plano y partir el query string. Nada de parser completo
// porque los cuerpos que mandamos son facilitos.
#pragma once

#include <string>
#include <map>

namespace json {

std::string escape(const std::string& s);
std::string str(const std::string& s);  // string con comillas y escapes

// Lee el valor string del campo "key" en un objeto JSON plano.
// Tolera espacios. Retorna "" si no existe. Maneja \" y \\ basicos.
std::string getString(const std::string& body, const std::string& key);

// Query string: "a=1&b=x%40y" -> mapa (aplica URL-decode, + -> espacio).
std::map<std::string, std::string> parseQuery(const std::string& qs);
std::string urlDecode(const std::string& s);

}  // namespace json
