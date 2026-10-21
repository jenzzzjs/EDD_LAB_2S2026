#pragma once
// WebSocket es un protocolo que empieza como una peticion HTTP normal. El
// navegador manda una peticion GET con las cabeceras Upgrade y Connection para
// pedir que la conexion cambie de protocolo. Si el servidor acepta, responde
// con el codigo 101 y desde ese momento el mismo socket TCP deja de hablar
// HTTP y empieza a transportar tramas WebSocket.
//
// El handshake usa SHA-1 y Base64. El servidor toma la clave Sec-WebSocket-Key
// que manda el cliente, le concatena un GUID fijo del estandar y calcula su
// SHA-1. El resultado se codifica en Base64 y se devuelve en la cabecera
// Sec-WebSocket-Accept. El navegador comprueba ese valor para confirmar que el
// servidor entendio el protocolo.
//
// Despues del handshake la comunicacion se hace con tramas. Cada trama tiene
// un primer byte con el bit FIN y el opcode, y un segundo byte con la mascara y
// el tamano. El cliente siempre enmascara lo que envia. El servidor nunca
// enmascara lo que envia. Los opcodes que usamos son texto, continuacion,
// cierre, ping y pong. Una trama puede venir partida en varias tramas
// pequenas, por eso el receptor une los fragmentos antes de entregar el
// mensaje completo.
//
// Este archivo solo declara las funciones basicas. La implementacion esta en
// WebSocket.cpp y el cambio de protocolo se hace en HttpServer.cpp.

#include <string>

namespace ws {

// Calcula el valor de Sec-WebSocket-Accept a partir de la clave del cliente.
// Primero concatena la clave con el GUID del estandar, despues calcula SHA-1 y
// al final codifica los 20 bytes en Base64.
std::string acceptKey(const std::string& clientKey);

// Envia una trama de texto. El servidor no aplica mascara a las tramas que
// salen, porque el estandar solo obliga a enmascarar al cliente.
bool sendText(unsigned long long sock, const std::string& texto);

// Envia una trama pong como respuesta a un ping del cliente.
bool sendPong(unsigned long long sock, const std::string& payload);

// Envia una trama de cierre con el codigo 1000, que significa cierre normal.
bool sendClose(unsigned long long sock);

// Lee una trama completa y deja el mensaje en salida. Soporta fragmentacion.
// Devuelve 1 cuando hay un mensaje, 0 cuando el cliente cerro la conexion y
// -1 cuando hubo un error de protocolo o de socket.
int recvMessage(unsigned long long sock, std::string& salida);

}  // namespace ws
