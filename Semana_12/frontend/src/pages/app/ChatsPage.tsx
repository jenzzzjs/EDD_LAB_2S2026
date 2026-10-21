import {
  FileArchive,
  MessageCircle,
  RefreshCw,
  Search,
  Send,
  ShieldCheck,
  Wifi,
  WifiOff,
} from 'lucide-react'
import { useCallback, useEffect, useMemo, useRef, useState } from 'react'
import type { FormEvent } from 'react'
import { Link, useSearchParams } from 'react-router-dom'
import { useAuth } from '../../lib/auth'
import { api } from '../../lib/api'
import type { Amigo, LzwInfo, MensajeChat } from '../../lib/api'
import { useChatSocket } from '../../lib/chat'
import type { ChatSocketEvent, ChatSocketStatus } from '../../lib/chat'
import { Avatar } from '../../components/atoms/Avatar'
import { Badge } from '../../components/atoms/Badge'
import { Card, CardHeader } from '../../components/atoms/Card'
import { Input } from '../../components/atoms/Input'
import { PageHeader } from '../../components/molecules/PageHeader'
import { useToast } from '../../components/molecules/Toast'
import { cn } from '../../lib/cn'

// Los mensajes en vivo pueden llegar mientras el usuario esta mirando otra
// conversacion. Esta funcion decide si un mensaje pertenece a la pareja que
// esta abierta en ese momento.
function esDeConversacion(mensaje: MensajeChat, yo: string, amigo: string): boolean {
  if (!yo || !amigo) return false
  return (
    (mensaje.de === yo && mensaje.para === amigo) ||
    (mensaje.de === amigo && mensaje.para === yo)
  )
}

// Unimos el historial que llega por HTTP con los mensajes que llegan por
// WebSocket. Usamos el id como clave para no duplicar el mismo mensaje. Esto
// pasa porque el servidor envia el evento al remitente y al destinatario, y
// porque una misma persona puede tener varias pestanas abiertas.
function unirMensajes(a: MensajeChat[], b: MensajeChat[]): MensajeChat[] {
  const mapa = new Map<string, MensajeChat>()
  for (const mensaje of [...a, ...b]) mapa.set(mensaje.id, mensaje)
  return Array.from(mapa.values()).sort((x, y) => {
    const nx = Number(x.hora)
    const ny = Number(y.hora)
    if (Number.isFinite(nx) && Number.isFinite(ny) && nx !== ny) return nx - ny
    return x.id.localeCompare(y.id)
  })
}

function formatearHora(hora: string): string {
  const numero = Number(hora)
  const fecha = Number.isFinite(numero) && numero > 0 ? new Date(numero) : new Date(hora)
  if (Number.isNaN(fecha.getTime())) return ''
  return fecha.toLocaleTimeString('es-GT', { hour: '2-digit', minute: '2-digit' })
}

// La tarjeta de LZW muestra los bytes originales y los comprimidos. Esta
// funcion solo cambia el formato para que sean mas faciles de leer.
function formatearBytes(bytes: number): string {
  if (bytes < 1024) return `${bytes} B`
  return `${(bytes / 1024).toFixed(1)} KB`
}

// Del path completo del archivo LZW mostramos solo el nombre. La ruta completa
// queda en el atributo title para que se pueda ver al pasar el mouse.
function nombreArchivo(ruta: string): string {
  return ruta.split(/[\\/]/).pop() ?? ruta
}

// Tres estados posibles de la conexion WebSocket. El color cambia segun si
// esta en linea, conectando o desconectado.
const estadoInfo: Record<
  ChatSocketStatus,
  { texto: string; tone: 'success' | 'warning' | 'danger' }
> = {
  en_linea: { texto: 'WebSocket en línea', tone: 'success' },
  conectando: { texto: 'Conectando…', tone: 'warning' },
  desconectado: { texto: 'Sin conexión', tone: 'danger' },
}

export function ChatsPage() {
  const { user } = useAuth()
  const correo = user?.email ?? ''
  const toast = useToast()
  const [searchParams, setSearchParams] = useSearchParams()

  const [amigos, setAmigos] = useState<Amigo[]>([])
  const [cargandoAmigos, setCargandoAmigos] = useState(true)
  const [query, setQuery] = useState('')
  // Aqui se juntan los mensajes que llegan por HTTP al abrir la conversacion y
  // los que llegan por WebSocket mientras la pagina esta abierta.
  const [mensajes, setMensajes] = useState<MensajeChat[]>([])
  // Datos del archivo LZW de la conversacion activa. El backend los calcula al
  // descomprimir el historial y al guardar cada mensaje nuevo.
  const [lzw, setLzw] = useState<LzwInfo | null>(null)
  const [cargandoHistorial, setCargandoHistorial] = useState(false)
  const [historialError, setHistorialError] = useState('')
  const [texto, setTexto] = useState('')
  const [noLeidos, setNoLeidos] = useState<Record<string, number>>({})
  const finRef = useRef<HTMLDivElement>(null)

  const amigoParam = searchParams.get('amigo') ?? ''
  const seleccionado = useMemo(
    () => amigos.find((amigo) => amigo.correo === amigoParam) ?? amigos[0] ?? null,
    [amigos, amigoParam],
  )
  const seleccionadoRef = useRef('')

  // Guardamos la conversacion actual en una referencia para que los callbacks
  // del historial y de WebSocket lean siempre el amigo vigente. Si usaramos el
  // estado directamente, una respuesta vieja podria escribir en la pantalla
  // equivocada.
  useEffect(() => {
    seleccionadoRef.current = seleccionado?.correo ?? ''
  }, [seleccionado])

  const cargarAmigos = useCallback(async () => {
    if (!correo) return
    setCargandoAmigos(true)
    try {
      const res = await api.amigos(correo)
      setAmigos(res.amigos)
    } catch (err) {
      toast.error(
        'No se pudieron cargar tus amigos',
        err instanceof Error ? err.message : undefined,
      )
    } finally {
      setCargandoAmigos(false)
    }
  }, [correo, toast])

  useEffect(() => {
    void cargarAmigos()
  }, [cargarAmigos])

  // Mantiene la conversacion elegida en la URL: al recargar la pagina se
  // vuelve a abrir el mismo chat.
  useEffect(() => {
    if (amigos.length === 0) return
    if (!amigoParam || !amigos.some((amigo) => amigo.correo === amigoParam)) {
      setSearchParams({ amigo: amigos[0].correo }, { replace: true })
    }
  }, [amigos, amigoParam, setSearchParams])

  // Pedimos el historial por HTTP. El backend busca el archivo LZW de la
  // pareja, lo descomprime y responde los mensajes y los datos de compresion.
  // Despues unimos esa respuesta con los mensajes que ya hayan llegado por
  // WebSocket para no perder ninguno.
  const cargarHistorial = useCallback(async () => {
    const amigo = seleccionado?.correo
    if (!correo || !amigo) {
      setMensajes([])
      setLzw(null)
      return
    }
    setCargandoHistorial(true)
    setHistorialError('')
    try {
      const data = await api.historialChat(correo, amigo)
      // Si el usuario cambio de amigo mientras llegaba la respuesta, no
      // escribimos el historial viejo en la conversacion nueva.
      if (seleccionadoRef.current !== amigo) return
      setMensajes((prev) =>
        unirMensajes(
          prev.filter((mensaje) => esDeConversacion(mensaje, correo, amigo)),
          data.mensajes,
        ),
      )
      setLzw(data.lzw)
    } catch (err) {
      if (seleccionadoRef.current === amigo) {
        setHistorialError(
          err instanceof Error ? err.message : 'No se pudo cargar el historial',
        )
      }
    } finally {
      if (seleccionadoRef.current === amigo) setCargandoHistorial(false)
    }
  }, [correo, seleccionado])

  // Cada vez que cambia la conversacion limpiamos los mensajes y los datos de
  // LZW de la anterior. Despues pedimos el historial de la nueva pareja.
  useEffect(() => {
    setMensajes([])
    setLzw(null)
    if (seleccionado) {
      setNoLeidos((prev) => ({ ...prev, [seleccionado.correo]: 0 }))
    }
    void cargarHistorial()
  }, [cargarHistorial, seleccionado])

  // Este callback recibe todos los eventos que llegan por WebSocket. Si el
  // mensaje pertenece a la conversacion abierta, lo agregamos a la lista y
  // actualizamos la tarjeta de LZW. Si es para el usuario pero de otra
  // conversacion, aumentamos el contador de no leidos.
  const manejarEvento = useCallback(
    (evento: ChatSocketEvent) => {
      if (evento.tipo === 'mensaje') {
        const mensaje = evento.mensaje
        const amigo = seleccionadoRef.current
        if (esDeConversacion(mensaje, correo, amigo)) {
          setMensajes((prev) => unirMensajes(prev, [mensaje]))
          setLzw(evento.lzw)
        } else if (mensaje.para === correo && mensaje.de !== correo) {
          const otro = mensaje.de
          setNoLeidos((prev) => ({ ...prev, [otro]: (prev[otro] ?? 0) + 1 }))
        }
      } else if (evento.tipo === 'error') {
        toast.error('Error de chat', evento.error)
      }
    },
    [correo, toast],
  )

  // El hook abre la conexion WebSocket y nos devuelve el estado actual y la
  // funcion para enviar mensajes. La conexion se mantiene mientras la pagina
  // este montada.
  const { estado, enviar } = useChatSocket(correo, manejarEvento)

  useEffect(() => {
    finRef.current?.scrollIntoView({ behavior: 'smooth', block: 'end' })
  }, [mensajes.length, seleccionado])

  const filtrados = useMemo(() => {
    const q = query.trim().toLowerCase()
    if (!q) return amigos
    return amigos.filter(
      (amigo) =>
        amigo.nombre.toLowerCase().includes(q) ||
        amigo.correo.toLowerCase().includes(q),
    )
  }, [amigos, query])

  // Enviamos el mensaje por WebSocket. El backend lo valida, lo comprime con
  // LZW y lo guarda. El limite es de 2000 bytes en UTF-8, no de caracteres,
  // porque un acento o un emoji puede ocupar mas de un byte.
  function enviarMensaje(event: FormEvent<HTMLFormElement>) {
    event.preventDefault()
    if (!seleccionado) return
    const limpio = texto.trim()
    if (!limpio) return
    if (new TextEncoder().encode(limpio).length > 2000) {
      toast.error('Mensaje demasiado largo', 'El máximo es 2000 bytes en UTF-8.')
      return
    }
    // Si el socket no esta abierto, el hook devuelve false y avisamos al
    // usuario para que no pierda el texto.
    if (!enviar(seleccionado.correo, limpio)) {
      toast.error(
        'WebSocket desconectado',
        'Espera a que se restablezca la conexión para enviar.',
      )
      return
    }
    setTexto('')
  }

  const infoEstado = estadoInfo[estado]

  return (
    <div className="mx-auto flex w-full max-w-[1180px] flex-col gap-6">
      <PageHeader
        breadcrumbs={[{ label: 'FaceJens' }, { label: 'Comunicación' }, { label: 'Chats' }]}
        eyebrow="Mensajería en tiempo real"
        title="Chats"
        description="Habla únicamente con tus amigos aceptados. Los mensajes viajan por WebSocket y cada conversación se guarda comprimida en LZW."
        chips={
          <>
            <Badge tone={infoEstado.tone}>
              <span
                className={cn(
                  'size-1.5 rounded-full',
                  estado === 'en_linea'
                    ? 'bg-success'
                    : estado === 'conectando'
                      ? 'bg-warning'
                      : 'bg-danger',
                )}
              />
              {infoEstado.texto}
            </Badge>
            <Badge tone="accent">
              <FileArchive size={12} />
              Historial LZW persistente
            </Badge>
            <Badge tone="neutral">
              <ShieldCheck size={12} />
              Solo amigos
            </Badge>
          </>
        }
      />

      {cargandoAmigos ? (
        <Card className="p-10 text-center">
          <p className="text-sm text-ink-muted">Cargando tus amigos…</p>
        </Card>
      ) : amigos.length === 0 ? (
        <Card className="flex flex-col items-center gap-3 p-10 text-center">
          <MessageCircle size={26} className="text-ink-tertiary" />
          <p className="text-sm font-semibold text-ink">
            Todavía no tienes amigos para chatear
          </p>
          <p className="max-w-sm text-[13px] leading-relaxed text-ink-subtle">
            El chat solo está disponible entre amistades aceptadas. Envía una
            solicitud desde Gestión y espera a que la acepten.
          </p>
          <Link
            to="/app/gestion"
            className="mt-1 inline-flex h-9 items-center rounded-lg bg-primary px-4 text-sm font-semibold text-white transition-colors hover:bg-primary-hover"
          >
            Ir a Gestión
          </Link>
        </Card>
      ) : (
        <div className="grid grid-cols-1 items-start gap-5 lg:grid-cols-[310px_minmax(0,1fr)]">
          {/* Lista de amigos */}
          <aside className="flex flex-col gap-4">
            <Card className="overflow-hidden">
              <div className="border-b border-hairline px-4 py-3">
                <CardHeader
                  title={`Amigos (${amigos.length})`}
                  icon={<MessageCircle size={15} className="text-accent" />}
                />
                <div className="mt-3">
                  <Input
                    value={query}
                    onChange={(event) => setQuery(event.target.value)}
                    placeholder="Buscar amigo…"
                    icon={<Search size={14} />}
                  />
                </div>
              </div>
              <ul className="max-h-[430px] overflow-y-auto p-2">
                {filtrados.length === 0 ? (
                  <li className="px-3 py-6 text-center text-xs text-ink-tertiary">
                    Sin coincidencias.
                  </li>
                ) : (
                  filtrados.map((amigo) => {
                    const activo = amigo.correo === seleccionado?.correo
                    const pendientes = noLeidos[amigo.correo] ?? 0
                    return (
                      <li key={amigo.correo}>
                        <button
                          type="button"
                          onClick={() => setSearchParams({ amigo: amigo.correo })}
                          className={cn(
                            'flex w-full items-center gap-3 rounded-lg px-2.5 py-2.5 text-left transition-colors',
                            activo
                              ? 'bg-surface-3'
                              : 'hover:bg-surface-2',
                          )}
                        >
                          <Avatar name={amigo.nombre} size="sm" />
                          <div className="min-w-0 flex-1">
                            <div className="flex items-center justify-between gap-2">
                              <span
                                className={cn(
                                  'truncate text-[13px] font-semibold',
                                  activo ? 'text-ink' : 'text-ink-muted',
                                )}
                              >
                                {amigo.nombre}
                              </span>
                              {pendientes > 0 ? (
                                <span className="grid min-w-5 shrink-0 place-items-center rounded-full bg-primary px-1 text-[10px] font-bold text-white">
                                  {pendientes}
                                </span>
                              ) : null}
                            </div>
                            <span className="block truncate font-mono text-[10px] text-ink-tertiary">
                              {amigo.correo}
                            </span>
                          </div>
                        </button>
                      </li>
                    )
                  })
                )}
              </ul>
            </Card>

            {/* Tarjeta de LZW. Muestra el archivo comprimido de la pareja,
                los bytes que tenia el texto original, los bytes que ocupa el
                archivo y el porcentaje de ahorro que calculo el backend. */}
            <Card className="p-4">
              <CardHeader
                title="Compresión LZW"
                icon={<FileArchive size={15} className="text-accent" />}
                action={
                  <Badge tone={lzw?.archivo ? 'success' : 'neutral'}>
                    {lzw?.archivo ? 'activo' : 'sin archivo'}
                  </Badge>
                }
              />
              {lzw?.archivo ? (
                <div className="mt-3 flex flex-col gap-2.5 text-xs">
                  <div className="flex items-center justify-between gap-2">
                    <span className="text-ink-tertiary">Archivo</span>
                    <span
                      className="max-w-[185px] truncate font-mono text-[10px] text-ink-subtle"
                      title={lzw.archivo}
                    >
                      {nombreArchivo(lzw.archivo)}
                    </span>
                  </div>
                  <div className="grid grid-cols-2 gap-2">
                    <div className="rounded-lg border border-hairline bg-canvas px-2.5 py-2">
                      <span className="block text-[10px] text-ink-tertiary">
                        Original
                      </span>
                      <span className="text-sm font-semibold text-ink">
                        {formatearBytes(lzw.bytes_original)}
                      </span>
                    </div>
                    <div className="rounded-lg border border-hairline bg-canvas px-2.5 py-2">
                      <span className="block text-[10px] text-ink-tertiary">
                        Comprimido
                      </span>
                      <span className="text-sm font-semibold text-accent">
                        {formatearBytes(lzw.bytes_comprimido)}
                      </span>
                    </div>
                  </div>
                  <p className="text-[11px] leading-relaxed text-ink-subtle">
                    {lzw.ahorro_porcentaje > 0
                      ? `${lzw.ahorro_porcentaje}% menos que el texto plano.`
                      : 'El archivo ya está en formato LZW; el ahorro aparece al acumular más mensajes.'}
                  </p>
                </div>
              ) : (
                <p className="mt-3 text-[11px] leading-relaxed text-ink-tertiary">
                  El archivo se crea en <span className="font-mono">lzw/</span> al
                  enviar el primer mensaje de esta conversación.
                </p>
              )}
              <p className="mt-3 border-t border-hairline pt-3 text-[10px] leading-relaxed text-ink-tertiary">
                Al abrir el chat se descomprime el archivo y al enviar se vuelve a
                comprimir con el historial completo.
              </p>
            </Card>
          </aside>

          {/* Conversación */}
          <section className="flex h-[620px] flex-col overflow-hidden rounded-xl border border-hairline bg-surface-1">
            {seleccionado ? (
              <>
                <header className="flex items-center gap-3 border-b border-hairline px-4 py-3">
                  <Avatar
                    name={seleccionado.nombre}
                    size="md"
                  />
                  <div className="min-w-0 flex-1">
                    <h2 className="truncate text-sm font-semibold text-ink">
                      {seleccionado.nombre}
                    </h2>
                    <p className="truncate font-mono text-[11px] text-ink-tertiary">
                      {seleccionado.correo}
                    </p>
                  </div>
                  <div className="flex items-center gap-2">
                    <Badge tone={infoEstado.tone}>
                      {estado === 'en_linea' ? (
                        <Wifi size={12} />
                      ) : (
                        <WifiOff size={12} />
                      )}
                      <span className="hidden sm:inline">{infoEstado.texto}</span>
                    </Badge>
                    <button
                      type="button"
                      onClick={() => void cargarHistorial()}
                      disabled={cargandoHistorial}
                      aria-label="Recargar historial"
                      title="Descomprimir de nuevo el historial LZW"
                      className="grid size-8 place-items-center rounded-lg border border-hairline text-ink-subtle transition-colors hover:bg-surface-2 hover:text-ink disabled:opacity-50"
                    >
                      <RefreshCw
                        size={14}
                        className={cargandoHistorial ? 'animate-spin' : ''}
                      />
                    </button>
                  </div>
                </header>

                <div className="flex-1 overflow-y-auto bg-canvas px-4 py-5">
                  {historialError ? (
                    <div className="mb-3 rounded-lg border border-danger/30 bg-danger/10 px-3 py-2 text-xs text-danger">
                      {historialError}
                    </div>
                  ) : null}

                  {cargandoHistorial && mensajes.length === 0 ? (
                    <div className="flex h-full flex-col items-center justify-center gap-2 text-center">
                      <RefreshCw size={18} className="animate-spin text-ink-tertiary" />
                      <p className="text-xs text-ink-tertiary">
                        Descomprimiendo historial LZW…
                      </p>
                    </div>
                  ) : mensajes.length === 0 ? (
                    <div className="flex h-full flex-col items-center justify-center gap-2 text-center">
                      <MessageCircle size={25} className="text-ink-tertiary" />
                      <p className="text-sm font-medium text-ink-muted">
                        Aún no hay mensajes
                      </p>
                      <p className="max-w-xs text-xs leading-relaxed text-ink-tertiary">
                        Escribe el primero. Se guardará en un archivo LZW y seguirá
                        aquí cuando recargues el chat.
                      </p>
                    </div>
                  ) : (
                    <div className="flex flex-col gap-2">
                      {mensajes.map((mensaje) => {
                        const propio = mensaje.de === correo
                        return (
                          <div
                            key={mensaje.id}
                            className={cn(
                              'flex',
                              propio ? 'justify-end' : 'justify-start',
                            )}
                          >
                            <div
                              className={cn(
                                'max-w-[82%] rounded-2xl px-3.5 py-2 text-sm leading-relaxed',
                                propio
                                  ? 'rounded-br-md bg-primary text-white'
                                  : 'rounded-bl-md border border-hairline bg-surface-2 text-ink',
                              )}
                            >
                              <p className="whitespace-pre-wrap break-words">
                                {mensaje.texto}
                              </p>
                              <span
                                className={cn(
                                  'mt-1 block text-right text-[10px]',
                                  propio ? 'text-white/60' : 'text-ink-tertiary',
                                )}
                              >
                                {formatearHora(mensaje.hora)}
                              </span>
                            </div>
                          </div>
                        )
                      })}
                      <div ref={finRef} />
                    </div>
                  )}
                </div>

                <form
                  onSubmit={enviarMensaje}
                  className="border-t border-hairline bg-surface-1 p-3"
                >
                  <div className="flex gap-2">
                    <input
                      value={texto}
                      onChange={(event) => setTexto(event.target.value)}
                      maxLength={2000}
                      placeholder="Escribe un mensaje…"
                      className="h-10 w-full rounded-lg border border-hairline bg-canvas px-3 text-sm text-ink transition-colors placeholder:text-ink-tertiary hover:border-hairline-strong focus:border-primary-focus focus:outline-none focus:ring-2 focus:ring-primary-focus/40"
                    />
                    <button
                      type="submit"
                      disabled={!texto.trim() || estado !== 'en_linea'}
                      className="inline-flex h-10 shrink-0 items-center justify-center gap-2 rounded-lg bg-primary px-4 text-sm font-semibold text-white transition-colors hover:bg-primary-hover disabled:pointer-events-none disabled:opacity-50"
                    >
                      <Send size={15} />
                      <span className="hidden sm:inline">Enviar</span>
                    </button>
                  </div>
                  <div className="mt-2 flex items-center justify-between gap-2 text-[10px] text-ink-tertiary">
                    <span>
                      {estado === 'en_linea'
                        ? 'Conectado por WebSocket'
                        : estado === 'conectando'
                          ? 'Conectando por WebSocket…'
                          : 'WebSocket desconectado'}
                    </span>
                    <span>{texto.length}/2000 · persistencia LZW</span>
                  </div>
                </form>
              </>
            ) : (
              <div className="flex h-full items-center justify-center p-10 text-sm text-ink-tertiary">
                Elige un amigo para empezar.
              </div>
            )}
          </section>
        </div>
      )}
    </div>
  )
}
