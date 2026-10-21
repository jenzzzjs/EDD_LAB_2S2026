#pragma once
// Base64 convierte bytes en caracteres ASCII. El handshake de WebSocket lo
// necesita porque SHA-1 produce 20 bytes que no se pueden mandar tal cual
// dentro de una cabecera HTTP. Base64 toma esos bytes de tres en tres y los
// representa con cuatro caracteres seguros. Si al final faltan bytes, completa
// el grupo con el signo igual.
//
// El navegador recibe ese texto en Sec-WebSocket-Accept y lo compara con el
// valor que espera. Si coincide, la conexion pasa de HTTP a WebSocket.

#include <string>

namespace b64 {

// Codifica los bytes recibidos en una cadena Base64.
std::string encode(const std::string& bytes);

}  // namespace b64
