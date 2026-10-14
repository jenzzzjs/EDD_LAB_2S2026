#pragma once
// Base64 encode hecho a mano (para meter el PNG dentro del JSON como texto,
// porque el JSON no puede llevar bytes crudos).

#include <string>

namespace b64 {

std::string encode(const std::string& bytes);

}  // namespace b64
