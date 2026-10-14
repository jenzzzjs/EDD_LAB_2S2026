# Semana 11 — Red social: documentacion de servicios

Sistema de red social con **frontend React** y **backend C++ (MVC)**. Este
documento describe los componentes de cada servicio, como se construyo y,
sobre todo, como se ejecuta.

```
+------------------+   HTTP JSON (:5000)   +------------------+
| FRONTEND (React) |  <----------------->  | BACKEND (C++ MVC)|
|  localhost:5173  |                       |  localhost:5000  |
+------------------+                       +------------------+
                                                      |
                                                      v
                                              Graphviz `dot`
                                              (DOT -> PNG)
```

---

## 1. Backend (`Semana_11/Backend/`)

### 1.1 Stack

- **Lenguaje:** C++17, sin frameworks (ni Crow, ni httplib, ni Boost).
- **HTTP:** servidor minimo escrito a mano sobre **Winsock**
  (`src/http/HttpServer.cpp`): sockets crudos, `GET`/`POST`/`OPTIONS`,
  un hilo por conexion y cabeceras CORS (`Access-Control-Allow-Origin: *`).
- **JSON / SHA-256 / Base64:** implementados a mano en `src/utils/`
  (sin nlohmann/json ni OpenSSL).
- **Grafo:** `GrafoNoDirigido` con lista de adyacencia hecha a mano
  (`std::map` como indice + `Lista` enlazada simple de vecinos, recorridos
  con ciclos explicitos y ordenamiento por burbuja). Sin `std::vector`,
  sin networkx ni ninguna libreria de grafos.
- **Graphviz:** solo renderiza. El backend genera el texto **DOT a mano** y
  lo convierte a PNG invocando el binario `dot -Tpng`. Requiere Graphviz
  instalado (`dot -V`).
- **Datos:** solo en memoria (lista de usuarios, lista de solicitudes, grafo).
  Al apagar el servidor todo se pierde. `POST /api/reset` vacia el estado
  (util para el laboratorio).

### 1.2 Componentes (MVC)

| Capa | Archivos | Responsabilidad |
|---|---|---|
| Modelo | `src/models/Lista.{h}` | `Lista<T>` enlazada simple hecha a mano (`new`/`delete`, cabeza + cola, `push_back`, `tam`, `operator[]`) + `Par` clave=valor: base de grafo, solicitudes, query strings y rutas (cero `std::vector`/`std::map`) |
| Modelo | `src/models/Grafo.{h,cpp}` | Grafo no dirigido: `agregarVertice`, `agregarArista` (ambos sentidos), `obtenerAmigos`, `amigosEnComun`, `redDeUsuario` (yo + directos + 2.º nivel), `toDotRed`, `toDotCompleto` |
| Modelo | `src/models/Store.{h,cpp}` | Singleton con `usuarios`, `solicitudes`, `grafo` y mutex (multihilo) |
| Controlador | `src/controllers/AuthController` | `registro(nombre, correo, password)`, `login(correo, password)` con hash SHA-256 |
| Controlador | `src/controllers/AmistadController` | `listarUsuarios`, `enviarSolicitud`, `pendientes`, `enviadas`, `aceptar` (crea la arista), `rechazar`, `listarAmigos` |
| Controlador | `src/controllers/GrafoController` | `redDeUsuario` (JSON + DOT + PNG en base64), `grafoCompleto`, `redPng` (bytes PNG), `estado` |
| Rutas | `src/routes/Routes` | Mapea cada `METODO + path` a su controlador |
| Vista | JSON / PNG | Es lo que consume el frontend (`application/json` e `image/png`) |
| `main` | `src/main.cpp` | Registra rutas y escucha en el puerto (defecto `5000`) |

### 1.3 API

| Metodo | Ruta | Cuerpo / query |
|---|---|---|
| GET | `/api/health` | estado |
| POST | `/api/reset` | vacia la memoria |
| POST | `/api/auth/registro` | `{nombre, correo, password}` (solo esos 3 datos) |
| POST | `/api/auth/login` | `{correo, password}` |
| GET | `/api/usuarios?excluir=&q=` | usuarios (para buscar a quien agregar) |
| GET | `/api/amigos?correo=` | amigos aceptados |
| POST | `/api/solicitudes` | `{de, para}` enviar (queda `pendiente`) |
| GET | `/api/solicitudes/pendientes?correo=` | recibidas pendientes |
| GET | `/api/solicitudes/enviadas?correo=` | enviadas pendientes |
| POST | `/api/solicitudes/aceptar` | `{de, para}` → `aceptada` + arista no dirigida |
| POST | `/api/solicitudes/rechazar` | `{de, para}` → `rechazada` |
| GET | `/api/grafo/red?correo=` | `{yo, amigos, amigos_en_comun, aristas, dot, imagen_base64}` |
| GET | `/api/grafo/red.png?correo=` | PNG directo (para `<img>`) |
| GET | `/api/grafo` | grafo completo + DOT + PNG base64 |

Colores de la red (`/api/grafo/red`): la persona **verde** `#22c55e`, sus
amigos directos **azul** `#2563eb`, amigos en comun (2.º nivel) **gris**
`#9ca3af`.

### 1.4 Como se construyo y como se ejecuta

Requisitos: **g++ con C++17** (MSYS2/MinGW) y **Graphviz** (`dot` en el PATH).

```bash
cd Semana_11/Backend

# Compilar (una sola linea, sin CMake ni dependencias que descargar)
g++ -std=c++17 -O2 -Isrc src/main.cpp src/utils/Json.cpp src/utils/Sha256.cpp \
  src/utils/Base64.cpp src/utils/Graphviz.cpp src/models/Grafo.cpp \
  src/models/Store.cpp src/controllers/Result.cpp \
  src/controllers/AuthController.cpp src/controllers/AmistadController.cpp \
  src/controllers/GrafoController.cpp src/http/HttpServer.cpp \
  src/routes/Routes.cpp -lws2_32 -o server.exe

# Ejecutar (puerto opcional, defecto 5000)
./server.exe 5000
# API en http://localhost:5000
```

Verificacion rapida:

```bash
curl http://localhost:5000/api/health
# {"ok":true,"servicio":"backend-semana11-mvc-cpp"}
```

---

## 2. Frontend (`Semana_11/frontend/`)

### 2.1 Stack

- **React 19 + TypeScript + Vite 8**, estilos con **Tailwind CSS 4**,
  rutas con **react-router-dom 7**, animaciones con **framer-motion**,
  iconos **lucide-react**.
- Sin `console.log` de depuracion en produccion; `npm run build` compila
  con `tsc -b && vite build` hacia `dist/`.

### 2.2 Componentes

| Parte | Archivos | Que hace |
|---|---|---|
| Cliente API | `src/lib/api.ts` | `fetch` al backend (`API_BASE`, defecto `http://localhost:5000`, sobreescribible con `VITE_API_URL`): registro, login, usuarios, amigos, solicitudes, red/grafo |
| Sesion | `src/lib/auth.tsx` | `AuthProvider`: `registerBackend(nombre, correo, password)`, `loginBackend(correo, password)`, `logout`; persiste la sesion en `localStorage` |
| Registro | `src/pages/RegisterPage.tsx` | Pide **unicamente** nombre completo, correo y contraseña → `POST /api/auth/registro` |
| Login | `src/pages/LoginPage.tsx` | Correo + contraseña → `POST /api/auth/login`, muestra errores del backend |
| Inicio (usuario) | `src/pages/app/InicioPage.tsx` | Resumen de la red + boton **“Ver red de amigos”** |
| Red | `src/pages/app/RedPage.tsx` | Lienzo SVG decorativo + tarjeta Graphviz real del backend |
| Gestion | `src/pages/app/GestionPage.tsx` | Enviar solicitud por correo, **pendientes con Aceptar/Rechazar** y lista de amigos (todo contra el API) |
| Visor de red | `src/components/organisms/app/RedAmigosViewer.tsx` | Pide `GET /api/grafo/red?correo=`, muestra el PNG con leyenda (verde = yo, azul = amigos, gris = en comun) y el DOT en un desplegable |
| Plantillas | `src/templates/` | `MarketingTemplate` (landing), `AuthTemplate` (login/registro), `AppTemplate` (zona privada, exige sesion) |
| Rutas | `src/router.tsx` | `/` landing, `/login`, `/registro`, `/app/inicio`, `/app/red`, `/app/gestion` |

### 2.3 Como se construyo y como se ejecuta

Requisitos: **Node 24+** (trae `npm`).

```bash
cd Semana_11/frontend

npm install        # una sola vez: instala dependencias
npm run dev        # desarrollo en http://localhost:5173
npm run build      # compila a dist/ (verifica tipos con tsc)
npm run preview    # sirve el build de produccion
```

> El frontend espera el backend en `http://localhost:5000`. Para cambiarlo:
> `VITE_API_URL=http://<host>:<puerto> npm run dev`.

---

## 3. Puesta en marcha completa (los dos servicios)

Terminal 1 — backend:

```bash
cd Semana_11/Backend
./server.exe 5000
```

Terminal 2 — frontend:

```bash
cd Semana_11/frontend
npm install
npm run dev
```

Abrir **http://localhost:5173**, registrarse (nombre, correo, password),
iniciar sesion, agregar amigos por correo desde **Gestion**, aceptar las
solicitudes y pulsar **“Ver red de amigos”** en **Inicio** para ver el grafo
Graphviz (verde = yo, azul = amigos, gris = amigos en comun).

Recordatorio: el backend no persiste; al cerrar `server.exe` los usuarios y
amistades se pierden y hay que registrarse de nuevo.
