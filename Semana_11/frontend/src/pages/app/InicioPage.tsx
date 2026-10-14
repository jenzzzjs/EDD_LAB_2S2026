import { Bell, Check, LayoutGrid, UserPlus } from 'lucide-react'
import { useCallback, useEffect, useMemo, useState } from 'react'
import type { ReactNode } from 'react'
import { Link } from 'react-router-dom'
import { useAuth } from '../../lib/auth'
import { api } from '../../lib/api'
import type { Amigo, SolicitudPendiente } from '../../lib/api'
import { Avatar } from '../../components/atoms/Avatar'
import { SearchBar } from '../../components/molecules/SearchBar'
import { useToast } from '../../components/molecules/Toast'
import { RedAmigosViewer } from '../../components/organisms/app/RedAmigosViewer'
import { cn } from '../../lib/cn'

interface Sugerido {
  nombre: string
  correo: string
}

type Tab = 'amigos' | 'solicitudes'

/** Anillo degradado estilo "historia" para destacar avatares. */
function StoryRing({ name, children }: { name: string; children: ReactNode }) {
  return (
    <span
      title={name}
      className="inline-flex shrink-0 rounded-full bg-gradient-to-tr from-[#f9ce34] via-[#ee2a7b] to-[#6228d7] p-[3px]"
    >
      <span className="rounded-full bg-canvas p-[2px]">{children}</span>
    </span>
  )
}

export function InicioPage() {
  const { user } = useAuth()
  const correo = user?.email ?? ''
  const [tab, setTab] = useState<Tab>('amigos')
  const [query, setQuery] = useState('')
  const [amigos, setAmigos] = useState<Amigo[]>([])
  const [sugeridos, setSugeridos] = useState<Sugerido[]>([])
  const [pendientes, setPendientes] = useState<SolicitudPendiente[]>([])
  const [enviadas, setEnviadas] = useState(0)
  const [enviadasCorreos, setEnviadasCorreos] = useState<string[]>([])
  const [enComun, setEnComun] = useState(0)
  const [enviando, setEnviando] = useState<string[]>([])
  const [cargando, setCargando] = useState(true)
  const toast = useToast()

  // Todo lo que la pagina necesita sale del backend de un solo jalon:
  // amigos, sugeridos (los que no son amigos), pendientes, enviadas y la
  // red para contar los en comun. Sin mocks: si el backend esta apagado,
  // se muestra vacio.
  const refrescar = useCallback(async () => {
    if (!correo) return
    try {
      const [amigosRes, usuariosRes, pendRes, envRes, redRes] = await Promise.all([
        api.amigos(correo),
        api.usuarios(correo),
        api.pendientes(correo),
        api.enviadas(correo),
        api.red(correo),
      ])
      setAmigos(amigosRes.amigos)
      setSugeridos(
        usuariosRes.usuarios.filter((u) => !u.es_amigo).map((u) => ({
          nombre: u.nombre,
          correo: u.correo,
        })),
      )
      setPendientes(pendRes.pendientes)
      setEnviadas(envRes.enviadas.length)
      setEnviadasCorreos(envRes.enviadas.map((e) => e.para))
      setEnComun(redRes.amigos_en_comun.length)
    } catch {
      /* backend apagado: se muestran estados vacios */
    } finally {
      setCargando(false)
    }
  }, [correo])

  useEffect(() => {
    void refrescar()
  }, [refrescar])

  const filtrados = useMemo(() => {
    const q = query.trim().toLowerCase()
    if (!q) return amigos
    return amigos.filter(
      (a) => a.nombre.toLowerCase().includes(q) || a.correo.toLowerCase().includes(q),
    )
  }, [amigos, query])

  // Añadir amigo = mandar solicitud, que queda pendiente hasta que el
  // acepte (ahi si nace la arista en el grafo del backend).
  async function conectar(destino: string) {
    toast.sending('Enviando solicitud…', destino)
    try {
      await api.enviarSolicitud(correo, destino)
      setEnviando((prev) => [...prev, destino])
      toast.success('Solicitud enviada', destino)
      await refrescar()
    } catch (err) {
      toast.error(
        'No se pudo enviar',
        err instanceof Error ? err.message : undefined,
      )
    }
  }

  // Responder una solicitud desde la pestana: aceptar crea la amistad,
  // rechazar la cierra. Despues se refresca todo para que cuadre.
  async function responder(de: string, accion: 'aceptar' | 'rechazar') {
    try {
      if (accion === 'aceptar') await api.aceptar(de, correo)
      else await api.rechazar(de, correo)
      toast.success(
        accion === 'aceptar' ? 'Amistad aceptada' : 'Solicitud rechazada',
        de,
      )
      await refrescar()
    } catch (err) {
      toast.error(
        'No se pudo responder',
        err instanceof Error ? err.message : undefined,
      )
    }
  }

  const stats = [
    { value: amigos.length, label: 'amigos' },
    { value: pendientes.length, label: 'pendientes' },
    { value: enComun, label: 'en común' },
  ]

  return (
    <div className="mx-auto flex w-full max-w-[935px] flex-col gap-8">
      {/* Cabecera de perfil */}
      <section className="flex flex-col gap-6 sm:flex-row sm:items-center sm:gap-10">
        <div className="flex justify-center sm:block">
          <StoryRing name={user?.name ?? ''}>
            <Avatar name={user?.name ?? '?'} size="lg" className="scale-[1.35]" online />
          </StoryRing>
        </div>

        <div className="flex min-w-0 flex-1 flex-col gap-4">
          <div className="flex flex-wrap items-center gap-x-4 gap-y-3">
            <h1 className="text-xl font-medium tracking-tight text-ink">
              {user?.handle ?? '@perfil'}
            </h1>
            <div className="flex gap-2">
              <Link
                to="/app/gestion"
                className="flex h-8 items-center rounded-lg bg-primary px-4 text-[13px] font-semibold text-white transition-colors hover:bg-primary-hover"
              >
                <UserPlus size={14} className="mr-1.5" />
                Agregar amigo
              </Link>
              <Link
                to="/app/red"
                className="flex h-8 items-center rounded-lg bg-surface-3 px-4 text-[13px] font-semibold text-ink transition-colors hover:bg-surface-4"
              >
                Ver red
              </Link>
            </div>
          </div>

          <ul className="flex items-center gap-6 text-[15px]">
            {stats.map((s) => (
              <li key={s.label} className="flex items-baseline gap-1.5">
                <strong className="font-semibold text-ink">{s.value}</strong>
                <span className="text-ink-subtle">{s.label}</span>
              </li>
            ))}
          </ul>

          <div className="flex flex-col leading-snug">
            <span className="text-sm font-semibold text-ink">{user?.name ?? ''}</span>
            <span className="font-mono text-[13px] text-ink-subtle">{correo}</span>
          </div>
        </div>
      </section>

      {/* Sugeridos estilo historias */}
      <section className="flex flex-col gap-3">
        <div className="flex items-center justify-between">
          <h2 className="text-sm font-semibold text-ink-subtle">Sugeridos para ti</h2>
          <Link to="/app/gestion" className="text-[13px] font-semibold text-accent hover:underline">
            Ver todos
          </Link>
        </div>
        {cargando ? (
          <p className="text-xs text-ink-tertiary">Cargando…</p>
        ) : sugeridos.length === 0 ? (
          <p className="text-xs text-ink-tertiary">No hay usuarios nuevos por ahora.</p>
        ) : (
          <div className="no-scrollbar -mx-1 flex gap-4 overflow-x-auto px-1 py-1">
            {sugeridos.slice(0, 10).map((person) => {
              const yaEnviada =
                enviando.includes(person.correo) || enviadasCorreos.includes(person.correo)
              const quiereConectar = pendientes.some((s) => s.de === person.correo)
              return (
                <div key={person.correo} className="flex w-28 shrink-0 flex-col items-center gap-1.5">
                  <StoryRing name={person.nombre}>
                    <Avatar name={person.nombre} size="md" />
                  </StoryRing>
                  <span className="w-full truncate text-center text-xs font-medium text-ink">
                    {person.nombre}
                  </span>
                  {yaEnviada ? (
                    <span className="flex h-7 items-center gap-1 rounded-lg bg-surface-3 px-3 text-xs font-semibold text-ink-tertiary">
                      <Check size={12} /> Enviada
                    </span>
                  ) : quiereConectar ? (
                    <Link
                      to="/app/gestion"
                      className="flex h-7 items-center rounded-lg bg-success/15 px-3 text-xs font-semibold text-success transition-colors hover:bg-success/25"
                    >
                      Responder
                    </Link>
                  ) : (
                    <button
                      type="button"
                      onClick={() => void conectar(person.correo)}
                      className="h-7 rounded-lg bg-primary px-3 text-xs font-semibold text-white transition-colors hover:bg-primary-hover"
                    >
                      Añadir amigo
                    </button>
                  )}
                </div>
              )
            })}
          </div>
        )}
      </section>

      {/* Pestañas */}
      <section className="flex flex-col">
        <div className="flex items-center justify-center gap-12 border-t border-hairline">
          <TabButton active={tab === 'amigos'} onClick={() => setTab('amigos')}>
            <LayoutGrid size={13} />
            AMIGOS
          </TabButton>
          <TabButton
            active={tab === 'solicitudes'}
            onClick={() => setTab('solicitudes')}
            badge={pendientes.length}
          >
            <Bell size={13} />
            SOLICITUDES
          </TabButton>
        </div>

        {tab === 'amigos' ? (
          <div className="flex flex-col gap-4 pt-5">
            <SearchBar
              value={query}
              onChange={setQuery}
              placeholder="Buscar amigos…"
              className="sm:max-w-xs"
            />
            {cargando ? (
              <p className="py-8 text-center text-sm text-ink-tertiary">Cargando tu red…</p>
            ) : filtrados.length === 0 ? (
              <div className="flex flex-col items-center gap-2 py-10 text-center">
                <p className="text-sm text-ink-muted">
                  {amigos.length === 0
                    ? 'Aún no tienes amigos. Envía tu primera solicitud.'
                    : 'Sin coincidencias.'}
                </p>
                {amigos.length === 0 ? (
                  <Link
                    to="/app/gestion"
                    className="text-[13px] font-semibold text-accent hover:underline"
                  >
                    Ir a Gestión
                  </Link>
                ) : null}
              </div>
            ) : (
              <>
                <div className="grid grid-cols-2 gap-3 sm:grid-cols-3">
                  {filtrados.map((amigo) => (
                    <div
                      key={amigo.correo}
                      className="flex flex-col items-center gap-1.5 rounded-xl border border-hairline bg-surface-1 px-3 py-6 text-center transition-colors hover:border-hairline-strong"
                    >
                      <Avatar name={amigo.nombre} size="md" online />
                      <span className="w-full truncate text-sm font-semibold text-ink">
                        {amigo.nombre}
                      </span>
                      <span className="w-full truncate font-mono text-[11px] text-ink-tertiary">
                        {amigo.correo}
                      </span>
                    </div>
                  ))}
                </div>
                <span className="text-center text-xs text-ink-tertiary">
                  {filtrados.length} de {amigos.length} amigos · {enviadas} enviadas sin respuesta
                </span>
              </>
            )}
          </div>
        ) : (
          <div className="pt-2">
            {pendientes.length === 0 ? (
              <p className="py-8 text-center text-sm text-ink-tertiary">
                Sin solicitudes por responder.
              </p>
            ) : (
              <ul className="divide-y divide-hairline border-y border-hairline">
                {pendientes.map((s) => (
                  <li key={s.de} className="flex items-center gap-3 py-3">
                    <Avatar name={s.nombre} size="sm" />
                    <div className="min-w-0 flex-1">
                      <div className="truncate text-sm font-semibold text-ink">{s.nombre}</div>
                      <div className="truncate text-[13px] text-ink-subtle">
                        quiere conectar contigo
                      </div>
                    </div>
                    <div className="flex shrink-0 gap-2">
                      <button
                        type="button"
                        onClick={() => void responder(s.de, 'aceptar')}
                        className="h-8 rounded-lg bg-primary px-4 text-[13px] font-semibold text-white transition-colors hover:bg-primary-hover"
                      >
                        Confirmar
                      </button>
                      <button
                        type="button"
                        onClick={() => void responder(s.de, 'rechazar')}
                        className="h-8 rounded-lg bg-surface-3 px-4 text-[13px] font-semibold text-ink transition-colors hover:bg-surface-4"
                      >
                        Eliminar
                      </button>
                    </div>
                  </li>
                ))}
              </ul>
            )}
          </div>
        )}
      </section>

      <RedAmigosViewer correo={correo} />
    </div>
  )
}

function TabButton({
  active,
  onClick,
  badge = 0,
  children,
}: {
  active: boolean
  onClick: () => void
  badge?: number
  children: ReactNode
}) {
  return (
    <button
      type="button"
      onClick={onClick}
      className={cn(
        '-mt-px flex items-center gap-1.5 border-t-2 pt-3 text-xs font-semibold tracking-[0.08em] transition-colors',
        active
          ? 'border-ink text-ink'
          : 'border-transparent text-ink-tertiary hover:text-ink-subtle',
      )}
    >
      {children}
      {badge > 0 ? (
        <span className="grid min-w-5 place-items-center rounded-full bg-primary px-1 text-[10px] font-bold text-white">
          {badge}
        </span>
      ) : null}
    </button>
  )
}
