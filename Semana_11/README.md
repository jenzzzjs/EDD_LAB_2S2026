# Semana 11 — Red social (C++ MVC + React)

App de red social: **Backend** en C++ (API JSON + PNG) y **frontend** en React (lo que ve el usuario). Detalle completo de ejecución en `Docs/Documentacion.md`.

```
frontend (:5173)  --HTTP JSON-->  Backend (:5000)  -->  dot (PNG)
```

## `Backend/` — API en C++17, sin frameworks

Se compila con `g++` a `backend.exe` (ver `Backend/README.md`). Datos solo en memoria: al apagar se pierden.

| Carpeta / archivo | Qué se trabaja ahí |
|---|---|
| `src/main.cpp` | Punto de entrada: lee el puerto (defecto `5000`), registra rutas y escucha. |
| `src/http/` | Servidor HTTP mínimo sobre Winsock (`GET`/`POST`/`OPTIONS`, CORS, un hilo por cliente). |
| `src/http/HttpServer.h` / `.cpp` | `route()`, `dispatch()`, `listen()`: tabla de rutas y ciclo accept. |
| `src/routes/` | Mapea cada `MÉTODO + path` (`/api/auth/...`, `/api/amigos`, `/api/grafo/...`) a su controlador. |
| `src/routes/Routes.h` / `.cpp` | `Routes::registrar()`: todo el ruteo del API en un solo lugar. |
| `src/models/` | Estructuras de datos del laboratorio (la parte de EDD). |
| `src/models/Grafo.h` / `.cpp` | `GrafoNoDirigido` con lista de adyacencia hecha a mano (`map` + `vector`): vértices, aristas en ambos sentidos, amigos en común, red de 2 niveles y generación del DOT. |
| `src/models/Store.h` / `.cpp` | Singleton en memoria: `usuarios`, `solicitudes`, `grafo` + `mutex` (multihilo). |
| `src/controllers/` | Lógica de cada endpoint: validan, tocan el `Store` y devuelven `Result`. |
| `src/controllers/Result.h` / `.cpp` | `{status, body, contentType}` + helpers (`errJson`, `trimStr`). |
| `src/controllers/AuthController.h` / `.cpp` | `registro(nombre, correo, password)` y `login` con SHA-256. |
| `src/controllers/AmistadController.h` / `.cpp` | Buscar usuarios, `enviarSolicitud`, `pendientes`/`enviadas`, `aceptar` (crea la arista), `rechazar`, `listarAmigos`. |
| `src/controllers/GrafoController.h` / `.cpp` | `redDeUsuario` (JSON + DOT + PNG base64), grafo completo y PNG directo. |
| `src/utils/` | Utilidades escritas a mano, sin librerías externas. |
| `src/utils/Json.h` / `.cpp` | Parseo/serialización JSON mínima + `parseQuery`. |
| `src/utils/Sha256.h` / `.cpp` | Hash SHA-256 para guardar passwords (nunca en claro). |
| `src/utils/Base64.h` / `.cpp` | Codifica el PNG a base64 para meterlo en el JSON. |
| `src/utils/Graphviz.h` / `.cpp` | Invoca el binario `dot -Tpng`: convierte el DOT hecho a mano en PNG. |
| `backend.exe` | Binario compilado (generado, no se edita). |

## `frontend/` — React 19 + TS + Vite + Tailwind

`npm install` → `npm run dev` (`:5173`) → `npm run build` (`dist/`). Config: `package.json` (deps), `vite.config.ts` (React + Tailwind), `tsconfig*.json` (tipos estrictos), `index.html` (título, fuentes, `#root`).

| Carpeta / archivo | Qué se trabaja ahí |
|---|---|
| `src/main.tsx` | Arranque: `AuthProvider` + `ToastProvider` + `RouterProvider`. |
| `src/router.tsx` | Rutas: `/` landing, `/login`, `/registro`, `/app/inicio`, `/app/red`, `/app/gestion`. |
| `src/index.css` | Tema global (canvas/superficies/tinta carmín, tipografías, utilidades). |
| `src/lib/api.ts` | Cliente HTTP del API (`API_BASE`, defecto `:5000`): auth, usuarios, amigos, solicitudes, red/grafo. |
| `src/lib/auth.tsx` | Sesión (`AuthProvider`, login/registro/logout, `localStorage`). |
| `src/lib/cn.ts` | `cn()`: une clases CSS condicionales. |
| `src/lib/types.ts` | Tipos compartidos (`Friend`, `GraphNode`). |
| `src/lib/data.ts` | Datos de ejemplo heredados. |
| `src/components/atoms/` | Piezas mínimas: `Avatar`, `Badge`, `Button`, `Card`, `Input`, `Logo`, `StatusDot`. |
| `src/components/molecules/` | Combinaciones: `SearchBar`, `FormField`, `PasswordField`/`PasswordStrength`, `PageHeader`, `PageTransition`, `Reveal`, `AnimatedNumber`, `StatTile`, `SegmentedControl`, `FormStagger` y `Toast` (notificaciones animadas arriba-derecha). |
| `src/components/organisms/app/AppHeader.tsx` | Barra superior de la zona privada. |
| `src/components/organisms/app/FriendCard.tsx` | Tarjeta de amigo (avatar, estado, acción). |
| `src/components/organisms/app/TopologyGraph.tsx` | Lienzo SVG interactivo de la malla (nodos por anillos, zoom, resonancia). |
| `src/components/organisms/app/RedAmigosViewer.tsx` | Tarjeta con la imagen PNG de la red (rectángulo fijo `object-contain`) + detalle plegable. |
| `src/components/organisms/marketing/` | Landing: `MarketingNav`, `HeroVisual`, `ProductPreview`, `TickerBand`. |
| `src/pages/LandingPage.tsx` | Portada pública. |
| `src/pages/LoginPage.tsx` / `RegisterPage.tsx` | Correo+password / nombre+correo+password contra el API. |
| `src/pages/app/InicioPage.tsx` | Perfil + sugeridos + tabs amigos/solicitudes + visor de red. |
| `src/pages/app/GestionPage.tsx` | Buscar personas, enviar solicitudes, aceptar/rechazar, lista de amigos. |
| `src/pages/app/RedPage.tsx` | Malla de conexiones: lienzo + telemetría + detalle de nodo + visor. |
| `src/templates/` | Marcos: `MarketingTemplate` (pública), `AuthTemplate` (login/registro), `AppTemplate` (privada, exige sesión). |

## `Docs/` — documentación

| Archivo | Qué contiene |
|---|---|
| `Docs/Documentacion.md` | Stack, MVC, tabla del API, cómo compilar/correr backend y frontend y puesta en marcha completa. |

## `ref/` — maquetas HTML de referencia

Bocetos estáticos previos al React final (no se ejecutan, solo consulta visual).

| Archivo | Qué muestra |
|---|---|
| `ref/Login.html` / `ref/Registro.html` | Maquetas de acceso y registro. |
| `ref/user/usuario.html` | Maqueta del perfil. |
| `ref/user/gestion.html` | Maqueta de gestión de solicitudes/amigos. |
| `ref/user/red.html` | Maqueta de la vista de red. |
