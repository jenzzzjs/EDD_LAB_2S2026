#pragma once
// SHA-256 implementado a mano (sin OpenSSL ni librerias externas).
// Se usa para hashear passwords; el formato hex es compatible con
// hashlib.sha256() de Python (es el mismo algoritmo estandar, asi que un
// hash hecho en Python se puede verificar aqui sin problema).

#include <string>

namespace sha256 {

std::string hashHex(const std::string& input);
bool verify(const std::string& plain, const std::string& hex);

}  // namespace sha256
