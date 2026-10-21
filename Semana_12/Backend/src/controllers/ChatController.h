#pragma once
// El historial se puede pedir por HTTP, pero los mensajes nuevos viajan por
// WebSocket. La conexion WebSocket se identifica con el correo del usuario y se
// queda abierta mientras la pestana siga activa. Cuando llega un mensaje, el
// controlador valida que los dos usuarios sean amigos, lo guarda comprimido con
// LZW y despues lo envia a todas las pestanas del remitente y del destinatario.
//
// La regla principal es que solo se puede chatear con amigos aceptados en el
// grafo de la Semana 11. El chat no confia en lo que manda el navegador, asi
// que vuelve a validar la amistad en cada mensaje.

#include <string>

#include "Result.h"

namespace ChatController {

// Devuelve el historial de una conversacion. El backend lee el archivo LZW,
// lo descomprime y responde con los mensajes y los datos de compresion.
Result historial(const std::string& correo, const std::string& amigo);

// Atiende una conexion WebSocket ya autenticada por la query ?correo=...
// Primero registra la conexion en el hub, despues lee tramas en un ciclo y
// termina cuando el cliente cierra o cuando ocurre un error.
void websocket(unsigned long long sock, const std::string& correo);

}  // namespace ChatController
