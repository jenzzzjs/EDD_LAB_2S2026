# Backend Semana 11 — MVC en C++ (red social)

Sin frameworks: HTTP con Winsock, JSON/SHA-256/Base64 escritos a mano.

- `src/models/` — `Store` (usuarios + solicitudes, **solo en memoria**, sin
  persistencia: al apagar el servidor los datos se pierden) y `Grafo`
  (`GrafoNoDirigido` con lista de adyacencia hecha a mano, sin librerias de grafos).
- `src/controllers/` — `AuthController` (registro/login),
  `AmistadController` (enviar/aceptar/rechazar/pendientes),
  `GrafoController` (red de usuario + DOT + PNG).
- `src/routes/` — rutas del API. `src/http/` — servidor HTTP minimo.
- `src/utils/` — `Json`, `Sha256`, `Base64` y `Graphviz` (solo invoca el
  binario `dot -Tpng`; el grafo se construye a mano, Graphviz solo renderiza).

## Requisitos

- g++ con C++17 (MSYS2/MinGW) o CMake 3.16+.
- Graphviz instalado (`dot -V`). Solo renderiza el DOT generado a mano.

## Compilar y ejecutar

```bash
g++ -std=c++17 -O2 -Isrc src/main.cpp src/utils/Json.cpp src/utils/Sha256.cpp \
  src/utils/Base64.cpp src/utils/Graphviz.cpp src/models/Grafo.cpp \
  src/models/Store.cpp src/controllers/Result.cpp \
  src/controllers/AuthController.cpp src/controllers/AmistadController.cpp \
  src/controllers/GrafoController.cpp src/http/HttpServer.cpp \
  src/routes/Routes.cpp -lws2_32 -o backend.exe

./backend.exe 5000
# API en http://localhost:5000 — frontend en http://localhost:5173
```

## API

| Metodo | Ruta | Cuerpo / query |
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
| GET | `/api/grafo/red?correo=` | JSON `{yo, amigos, amigos_en_comun, aristas, dot, imagen_base64}` |
| GET | `/api/grafo/red.png?correo=` | PNG directo (para `<img>`) |
| GET | `/api/grafo` | grafo completo + DOT + PNG base64 |

Colores de `GET /api/grafo/red`: yo **verde** `#22c55e`, amigos directos
**azul** `#2563eb`, amigos en comun **gris** `#9ca3af`.
