#pragma once
// WebSocket no usa SHA-1 para guardar contrasenas. Lo usa una sola vez durante
// el handshake, porque el estandar RFC 6455 pide calcular SHA-1 sobre la clave
// que manda el cliente mas un GUID fijo. El resultado son 20 bytes crudos que
// despues se codifican en Base64 y se devuelven en Sec-WebSocket-Accept.
//
// Las contrasenas de la aplicacion siguen usando SHA-256. SHA-1 esta aqui
// unicamente porque el protocolo WebSocket lo exige y el navegador compara el
// valor para confirmar que el servidor entendio la solicitud de cambio de
// protocolo.

#include <string>

namespace sha1 {

// Calcula el digest SHA-1 del texto de entrada. Devuelve 20 bytes crudos, no
// una cadena hexadecimal. El handshake necesita esos bytes para pasarlos a
// Base64.
std::string digest(const std::string& input);

}  // namespace sha1
