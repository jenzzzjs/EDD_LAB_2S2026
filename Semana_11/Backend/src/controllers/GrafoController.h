#pragma once
#include <string>
#include "Result.h"

namespace GrafoController {

Result redDeUsuario(const std::string& correo);
Result grafoCompleto();
Result redPng(const std::string& correo);  // contentType image/png
Result estado();

}  // namespace GrafoController
