#pragma once
#include <string>
#include "Result.h"

namespace AmistadController {

Result listarUsuarios(const std::string& excluir, const std::string& q);
Result enviarSolicitud(const std::string& de, const std::string& para);
Result pendientes(const std::string& correo);
Result enviadas(const std::string& correo);
Result aceptar(const std::string& de, const std::string& para);
Result rechazar(const std::string& de, const std::string& para);
Result listarAmigos(const std::string& correo);

}  // namespace AmistadController
