# Backend Semana 12 — red social + chat con WebSockets y LZW

Extiende el backend MVC en C++ de la Semana 11 con:

- **WebSocket nativo** (RFC 6455) en el mismo puerto del HTTP, sin librerías externas.
- **Chat solo entre amigos**: cada mensaje y cada historial se validan contra el
  grafo de amistades (`GrafoNoDirigido::sonAmigos`).
- **Persistencia en LZW**: cada conversación se guarda comprimida en
  `lzw/chat_<correoA>__<correoB>_<hash>.lzw` (correos ordenados) y se
  descomprime al abrir el chat.

## Archivos nuevos respecto a la Semana 11

| Archivo | Función |
|---|---|
| `src/http/WebSocket.h` | Declaraciones del handshake y de las tramas WebSocket. |
| `src/http/WebSocket.cpp` | Handshake, masking, ping/pong, cierre y fragmentación. |
| `src/controllers/ChatController.h` | Interfaz del historial y de la conexión WebSocket. |
| `src/controllers/ChatController.cpp` | Hub de conexiones, envío en tiempo real y validación de amistad. |
| `src/models/ChatStore.h` | Interfaz de persistencia de conversaciones en LZW. |
| `src/models/ChatStore.cpp` | Serialización, compresión, descompresión y escritura de archivos `.lzw`. |
| `src/models/Mensaje.h` | Estructura de un mensaje y metadatos de compresión. |
| `src/utils/Lzw.h` | Declaraciones del compresor y descompresor LZW. |
| `src/utils/Lzw.cpp` | Diccionario LZW, códigos de 12 bits y empaquetado en bytes. |
| `src/utils/Sha1.h` | Declaración de SHA-1 para el handshake. |
| `src/utils/Sha1.cpp` | Implementación de SHA-1 usada en `Sec-WebSocket-Accept`. |

`src/utils/Base64` ya existía en la Semana 11 y se reutiliza para codificar el
resultado de SHA-1.

## Estructura

- `src/models/Store` — usuarios, solicitudes y grafo (**en memoria**, como en la
  Semana 11).
- `src/models/Grafo` — grafo no dirigido hecho a mano.
- `src/models/ChatStore` — carga/guarda conversaciones en LZW. Mantiene una
  copia en memoria y reescribe el archivo completo al enviar un mensaje.
- `src/controllers/ChatController` — historial HTTP, envío en tiempo real,
  validación de amistad y registro de conexiones WebSocket.
- `src/http/HttpServer` — HTTP/1.1 mínimo + handshake WebSocket en `/ws`.
- `src/http/WebSocket` — tramas, ping/pong, close, fragmentación y handshake.
- `src/utils/Lzw` — compresión/descompresión LZW de 12 bits, hecha a mano.
- `src/utils/Sha1` — SHA-1 usado únicamente para `Sec-WebSocket-Accept`.
- `src/utils/Json`, `Sha256`, `Base64`, `Graphviz` — utilidades existentes.
- `lzw/` — carpeta donde se crean los archivos de las conversaciones.

## Requisitos

- g++ con C++17 (MSYS2/MinGW).
- Graphviz instalado (`dot -V`) para el PNG de la red. No se usa en el chat.

## Compilar y ejecutar

Ejecutar desde `Semana_12/Backend`, para que la carpeta `lzw/` quede junto al
backend:

```bash
g++ -std=c++17 -O2 -Isrc src/main.cpp \
  src/utils/Json.cpp src/utils/Sha256.cpp src/utils/Base64.cpp \
  src/utils/Graphviz.cpp src/utils/Lzw.cpp src/utils/Sha1.cpp \
  src/models/Grafo.cpp src/models/Store.cpp src/models/ChatStore.cpp \
  src/controllers/Result.cpp src/controllers/AuthController.cpp \
  src/controllers/AmistadController.cpp src/controllers/GrafoController.cpp \
  src/controllers/ChatController.cpp src/http/HttpServer.cpp \
  src/http/WebSocket.cpp src/routes/Routes.cpp \
  -lws2_32 -o backend.exe

./backend.exe 5000
# HTTP: http://localhost:5000
# WebSocket: ws://localhost:5000/ws?correo=usuario@correo.com
```

Al arrancar, el backend imprime la ruta de la carpeta LZW y crea `lzw/` si no
existe. En PowerShell, pega el comando de compilación en una sola línea (sin las
`\` de continuación).

## API HTTP

| Método | Ruta | Cuerpo / query |
|---|---|---|
| POST | `/api/auth/registro` | `{nombre, correo, password}` |
| POST | `/api/auth/login` | `{correo, password}` |
| GET | `/api/usuarios?excluir=&q=` | lista usuarios |
| GET | `/api/amigos?correo=` | amigos aceptados |
| POST | `/api/solicitudes` | `{de, para}` enviar |
| GET | `/api/solicitudes/pendientes?correo=` | recibidas pendientes |
| GET | `/api/solicitudes/enviadas?correo=` | enviadas pendientes |
| POST | `/api/solicitudes/aceptar` | `{de, para}` |
| POST | `/api/solicitudes/rechazar` | `{de, para}` |
| GET | `/api/grafo/red?correo=` | JSON de la red + DOT + PNG base64 |
| GET | `/api/grafo/red.png?correo=` | PNG directo |
| GET | `/api/grafo` | grafo completo + DOT + PNG base64 |
| GET | `/api/chats/historial?correo=&amigo=` | historial descomprimido + metadatos LZW |

`GET /api/chats/historial` responde:

```json
{
  "ok": true,
  "correo": "ana@test.com",
  "amigo": "bruno@test.com",
  "mensajes": [
    {
      "id": "1792472034485-1",
      "de": "ana@test.com",
      "para": "bruno@test.com",
      "texto": "Hola Bruno",
      "hora": "1792472034485"
    }
  ],
  "lzw": {
    "archivo": "lzw/chat_ana_at_test.com__bruno_at_test.com_db0f48bdfa.lzw",
    "bytes_original": 436,
    "bytes_comprimido": 398,
    "ahorro_porcentaje": 8
  }
}
```

Si los usuarios no son amigos, responde `403` con
`{"ok":false,"error":"solo puedes chatear con amigos"}`. También se valida en
cada mensaje WebSocket, no solo en la interfaz.

## WebSocket

Conectar a:

```text
ws://localhost:5000/ws?correo=ana@test.com
```

El backend verifica que el correo exista. El cliente debe enviar tramas de texto
JSON.

### Cliente → servidor

```json
{"tipo":"mensaje","para":"bruno@test.com","texto":"Hola"}
{"tipo":"historial","amigo":"bruno@test.com"}
{"tipo":"ping"}
```

### Servidor → cliente

```json
{"tipo":"conectado","correo":"ana@test.com"}
{"tipo":"mensaje","mensaje":{...},"lzw":{"archivo":"lzw/...","bytes_original":436,"bytes_comprimido":398,"ahorro_porcentaje":8}}
{"tipo":"historial","amigo":"bruno@test.com","mensajes":[...],"lzw":{...}}
{"tipo":"error","error":"solo puedes chatear con amigos"}
{"tipo":"pong"}
```

El evento `mensaje` se envía a todas las pestañas conectadas del emisor y del
receptor. Así el emisor también recibe el mensaje confirmado por el servidor y
no se duplica al recargar el historial.

## Formato LZW

Cada archivo `.lzw` contiene:

```text
"LZW1"                    magic de 4 bytes
uint64 little-endian      tamaño del texto original
códigos de 12 bits        empaquetados en bytes consecutivos
```

- Códigos `0..255`: bytes literales.
- Código `256`: `CLEAR` (reinicia el diccionario).
- Códigos `257..4095`: secuencias del diccionario.

El texto original es una línea JSON por mensaje (`id`, `de`, `para`, `texto`,
`hora`). Al cargar el historial se descomprime y se parsea; al enviar un mensaje
se vuelve a serializar y comprimir toda la conversación. Los archivos están en
`lzw/` y sobreviven al reinicio del backend. Los usuarios y amistades siguen en
memoria como en la Semana 11, así que para ver un historial antiguo después de
reiniciar hay que volver a registrar los mismos correos y aceptar la amistad.

## Pruebas rápidas

```bash
# Salud
curl http://localhost:5000/api/health

# Historial (con dos usuarios que ya sean amigos)
curl "http://localhost:5000/api/chats/historial?correo=ana@test.com&amigo=bruno@test.com"
```

El WebSocket se puede probar desde la vista **Chats** del frontend, o con
cualquier cliente que soporte `ws://` (Node, navegador, `websocat`, etc.).
