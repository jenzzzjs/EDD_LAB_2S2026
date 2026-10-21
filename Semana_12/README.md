# Semana 12 — Chat con WebSockets y compresión LZW

Esta semana parte del proyecto de la Semana 11 (red social con grafo de
amistades) y agrega:

- Comunicación en tiempo real con **WebSocket** nativo en el backend C++.
- Vista de **Chats** en React, disponible solo para amigos aceptados.
- Persistencia de cada conversación en un archivo **LZW** dentro de `Backend/lzw/`.
- Descompresión del historial al abrir el chat y recompresión al enviar mensajes.

## Archivos nuevos respecto a la Semana 11

Estos archivos no existían en `Semana_11` y se crearon para la Semana 12:

| Archivo | Para qué sirve |
|---|---|
| `Backend/src/http/WebSocket.h` | Declara el handshake, el envío de tramas y la recepción de mensajes WebSocket. |
| `Backend/src/http/WebSocket.cpp` | Implementa el protocolo WebSocket: handshake, masking, ping/pong, cierre y fragmentación. |
| `Backend/src/controllers/ChatController.h` | Declara el historial HTTP y la atención de una conexión WebSocket. |
| `Backend/src/controllers/ChatController.cpp` | Registra las conexiones activas, valida amistades, envía mensajes en tiempo real y devuelve historiales. |
| `Backend/src/models/ChatStore.h` | Declara la persistencia de conversaciones comprimidas con LZW. |
| `Backend/src/models/ChatStore.cpp` | Serializa, comprime, descomprime y guarda cada conversación en `lzw/`. |
| `Backend/src/models/Mensaje.h` | Define un mensaje de chat y los metadatos de compresión LZW. |
| `Backend/src/utils/Lzw.h` | Declara las funciones de compresión y descompresión LZW. |
| `Backend/src/utils/Lzw.cpp` | Implementa el diccionario LZW, los códigos de 12 bits y el empaquetado en bytes. |
| `Backend/src/utils/Sha1.h` | Declara SHA-1 para calcular `Sec-WebSocket-Accept`. |
| `Backend/src/utils/Sha1.cpp` | Implementa SHA-1, usado únicamente durante el handshake de WebSocket. |
| `frontend/src/lib/chat.ts` | Cliente WebSocket con reconexión automática y envío de eventos. |
| `frontend/src/pages/app/ChatsPage.tsx` | Vista de chats, historial, mensajes en vivo y tarjeta de compresión LZW. |

También se modificaron archivos que ya existían en la Semana 11:

| Archivo | Cambio |
|---|---|
| `Backend/src/http/HttpServer.h` | Se agregó el registro de la ruta WebSocket. |
| `Backend/src/http/HttpServer.cpp` | Se agregó el handshake HTTP a WebSocket y el cambio de protocolo en el mismo puerto. |
| `Backend/src/routes/Routes.cpp` | Se registraron `/api/chats/historial` y `/ws`. |
| `Backend/src/utils/Base64.h` y `Base64.cpp` | Se documentó su uso en el handshake WebSocket; el codificador ya existía. |
| `Backend/src/main.cpp` | Se inicializa `ChatStore` y se imprimen las rutas del WebSocket y de los archivos LZW. |
| `frontend/src/router.tsx` | Se agregó la ruta de la página de chats. |
| `frontend/src/components/organisms/app/AppHeader.tsx` | Se agregó el acceso a Chats en la navegación. |
| `frontend/src/lib/api.ts` | Se agregaron los tipos y la llamada del historial de chat. |
| `frontend/src/pages/app/InicioPage.tsx` | Se agregó el acceso directo para chatear con un amigo desde el inicio. |

`Backend/src/utils/Base64.cpp` y `Base64.h` ya existían en la Semana 11. En la
Semana 12 se reutilizan para codificar el resultado de SHA-1 en el handshake.

## Estructura

```text
Semana_12/
├── Backend/
│   ├── src/
│   │   ├── controllers/ChatController.*   # historial + WebSocket
│   │   ├── http/WebSocket.*               # RFC 6455 hecho a mano
│   │   ├── models/ChatStore.*             # persistencia LZW
│   │   ├── utils/Lzw.*                    # compresión/descompresión
│   │   └── utils/Sha1.*                   # handshake WebSocket
│   ├── lzw/                               # archivos .lzw generados
│   └── README.md                          # API, protocolo y formato
└── frontend/
    └── src/
        ├── lib/chat.ts                    # cliente WebSocket
        └── pages/app/ChatsPage.tsx        # vista de chats
```

## Ejecutar

### Backend

Desde `Semana_12/Backend`:

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
```

### Frontend

Desde `Semana_12/frontend`:

```bash
npm install
npm run dev
```

Abrir `http://localhost:5173`, iniciar sesión y entrar a **Chats** en la barra de
navegación. Si el backend usa otro puerto, definir `VITE_API_URL`, por ejemplo:

```bash
VITE_API_URL=http://localhost:5001 npm run dev
```

En PowerShell: `$env:VITE_API_URL="http://localhost:5001"; npm run dev`.

## Cómo funciona el chat

1. Solo aparecen en la lista los amigos aceptados (`GET /api/amigos`).
2. Al elegir un amigo se pide `GET /api/chats/historial`, que lee el `.lzw`,
   lo descomprime y devuelve los mensajes.
3. Al enviar, el navegador manda un evento por WebSocket:
   `{"tipo":"mensaje","para":"amigo@correo.com","texto":"..."}`.
4. El backend valida que ambos usuarios existan y que sean amigos; si no, rechaza
   el mensaje aunque el cliente intente saltarse la interfaz.
5. Si es válido, agrega el mensaje, recompone el archivo LZW de la pareja y
   difunde el evento a las pestañas del emisor y del receptor.
6. Al recargar la página, el historial se vuelve a descomprimir desde `lzw/`.

## Persistencia

- Los archivos `.lzw` quedan en `Semana_12/Backend/lzw/`.
- Sobreviven al reinicio del backend.
- Los usuarios, solicitudes y amistades siguen en memoria (igual que en la
  Semana 11). Después de reiniciar el backend hay que volver a registrar los
  mismos correos y aceptar la amistad para que el chat vuelva a habilitarse; los
  mensajes anteriores se cargan desde el archivo LZW.

## Documentación detallada

El protocolo WebSocket completo, las rutas HTTP y el formato binario LZW están
en [`Backend/README.md`](Backend/README.md).
