# Frontend Semana 12 — FaceJens

Interfaz React + TypeScript + Vite para la red social de la Semana 11, con la
nueva vista de **Chats** por WebSocket.

## Ejecutar

```bash
npm install
npm run dev
```

La app queda en `http://localhost:5173`. Por defecto consume el backend en
`http://localhost:5000`; si el backend usa otro puerto:

```bash
VITE_API_URL=http://localhost:5001 npm run dev
```

## Compilar y revisar

```bash
npm run build
npm run lint
```

## Chats

- Entrar a **Chats** desde la barra de navegación (`/app/chats`).
- Solo aparecen los amigos aceptados.
- Los mensajes nuevos se envían por WebSocket (`lib/chat.ts`).
- El historial se carga por HTTP (`GET /api/chats/historial`) desde los
  archivos LZW del backend.
- La conversación elegida se conserva en la URL con `?amigo=correo@...`.

La documentación completa del protocolo y del formato LZW está en
[`../Backend/README.md`](../Backend/README.md).
