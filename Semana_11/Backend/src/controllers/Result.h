#pragma once
// Lo que devuelve un controlador: codigo HTTP + cuerpo + tipo de contenido.
// Casi todo es JSON, menos el PNG de la red que va como image/png.

#include <string>

struct Result {
    int status = 200;
    std::string body;
    std::string contentType = "application/json";
};

std::string toLowerStr(const std::string& s);
std::string trimStr(const std::string& s);
std::string errJson(const std::string& msg);
