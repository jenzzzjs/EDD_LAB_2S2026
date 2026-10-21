#pragma once
#include <string>
#include "Result.h"

namespace AuthController {

Result registro(const std::string& nombre, const std::string& correo,
                const std::string& password);
Result login(const std::string& correo, const std::string& password);

}  // namespace AuthController
