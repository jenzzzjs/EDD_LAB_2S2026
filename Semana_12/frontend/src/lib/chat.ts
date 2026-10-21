import { useCallback, useEffect, useRef, useState } from 'react'
import type { LzwInfo, MensajeChat } from './api'
import { API_BASE } from './api'

// Estado de la conexion WebSocket tal como lo muestra la pagina de chats.
export type ChatSocketStatus = 'conectando' | 'en_linea' | 'desconectado'

// Eventos que el servidor puede enviar por WebSocket. El campo tipo funciona
// como discriminante para que la pagina sepa que campos leer en cada caso.
export type ChatSocketEvent =
  | { tipo: 'conectado'; correo: string }
  | { tipo: 'mensaje'; mensaje: MensajeChat; lzw: LzwInfo }
  | { tipo: 'historial'; amigo: string; mensajes: MensajeChat[]; lzw: LzwInfo }
  | { tipo: 'error'; error: string }
  | { tipo: 'pong' }

// Cliente WebSocket del chat. La pagina monta este hook una sola vez y el hook
// se encarga de abrir la conexion, entregar los eventos, reconectar si se cae y
// cerrar el socket cuando la pagina se desmonta.

/** La misma direccion del API, pero con esquema ws o wss. */
export function chatSocketUrl(correo: string): string {
  // WebSocket empieza como una peticion HTTP. Si el API usa http cambiamos a
  // ws y si usa https cambiamos a wss. El correo viaja en la query para que el
  // backend sepa a que usuario pertenece la conexion.
  const base = API_BASE.replace(/^http/i, 'ws')
  return `${base}/ws?correo=${encodeURIComponent(correo)}`
}

/**
 * Mantiene una conexion WebSocket abierta mientras la pagina de chats este
 * montada. Los eventos que llegan se entregan al callback de la pagina. Si la
 * conexion se cierra, el hook intenta reconectarse con una espera que crece
 * poco a poco para no saturar al servidor.
 */
export function useChatSocket(
  correo: string,
  onEvent: (evento: ChatSocketEvent) => void,
): {
  estado: ChatSocketStatus
  enviar: (para: string, texto: string) => boolean
} {
  // Guardamos el callback en una referencia para que el socket no tenga que
  // cerrarse y abrirse cada vez que la pagina crea una funcion nueva.
  const eventoRef = useRef(onEvent)
  const socketRef = useRef<WebSocket | null>(null)
  const [estado, setEstado] = useState<ChatSocketStatus>(
    correo ? 'conectando' : 'desconectado',
  )

  useEffect(() => {
    eventoRef.current = onEvent
  }, [onEvent])

  // Enviamos un mensaje solo si el socket esta abierto. Si todavia se esta
  // conectando o ya se cerro, devolvemos false para que la pagina muestre un
  // aviso en lugar de perder el texto.
  const enviar = useCallback((para: string, texto: string) => {
    const ws = socketRef.current
    if (!ws || ws.readyState !== WebSocket.OPEN) return false
    ws.send(JSON.stringify({ tipo: 'mensaje', para, texto }))
    return true
  }, [])

  // Este efecto se ejecuta cuando cambia el correo y se encarga de toda la
  // vida de la conexion. Mientras activo sea true, el socket pertenece a esta
  // pagina. Cuando el componente se desmonta, activo pasa a false y ya no se
  // programa otra reconexion.
  useEffect(() => {
    if (!correo) return

    let activo = true
    let intento = 0
    let reconexion: ReturnType<typeof setTimeout> | null = null

    const conectar = () => {
      if (!activo) return
      setEstado('conectando')
      let ws: WebSocket
      try {
        ws = new WebSocket(chatSocketUrl(correo))
      } catch {
        // Si el navegador no puede crear el socket, esperamos un poco y
        // volvemos a intentar. La espera se duplica en cada intento hasta un
        // maximo de diez segundos.
        setEstado('desconectado')
        const espera = Math.min(1000 * 2 ** intento, 10000)
        intento += 1
        reconexion = setTimeout(conectar, espera)
        return
      }
      socketRef.current = ws

      ws.onopen = () => {
        // La conexion ya paso el handshake y el servidor puede enviar y
        // recibir tramas. Reiniciamos el contador de intentos.
        if (!activo) return
        intento = 0
        setEstado('en_linea')
      }

      ws.onmessage = (evento) => {
        // Cada trama de texto trae un evento JSON. Si llega algo que no es
        // JSON, lo ignoramos para no romper la pagina.
        try {
          const data = JSON.parse(String(evento.data)) as ChatSocketEvent
          if (data && typeof data.tipo === 'string') eventoRef.current(data)
        } catch {
          // La trama no tiene el formato esperado y se descarta.
        }
      }

      ws.onclose = () => {
        // El servidor cerro la conexion o se perdio la red. Marcamos el estado
        // y programamos otro intento con espera exponencial.
        if (!activo) return
        socketRef.current = null
        setEstado('desconectado')
        const espera = Math.min(1000 * 2 ** intento, 10000)
        intento += 1
        reconexion = setTimeout(conectar, espera)
      }

      ws.onerror = () => {
        // El navegador dispara onclose despues de onerror, asi que la
        // reconexion se programa desde alli para no duplicar intentos.
      }
    }

    conectar()

    return () => {
      // Al desmontar la pagina cerramos el socket. Primero quitamos onclose
      // para que el cierre normal no dispare una reconexion.
      activo = false
      if (reconexion) clearTimeout(reconexion)
      const ws = socketRef.current
      socketRef.current = null
      if (ws) {
        ws.onclose = null
        try {
          ws.close()
        } catch {
          // El socket ya estaba cerrado.
        }
      }
    }
  }, [correo])

  return { estado: correo ? estado : 'desconectado', enviar }
}
