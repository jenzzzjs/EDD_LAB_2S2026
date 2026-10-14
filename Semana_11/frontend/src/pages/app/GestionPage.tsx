import { Check, Search, Trash2, UserMinus, Users } from 'lucide-react'
import { AnimatePresence, motion } from 'framer-motion'
import { useCallback, useEffect, useMemo, useState } from 'react'
import type { Friend } from '../../lib/types'
import { useAuth } from '../../lib/auth'
import { api } from '../../lib/api'
import type { SolicitudPendiente } from '../../lib/api'
import { Avatar } from '../../components/atoms/Avatar'
import { Button } from '../../components/atoms/Button'
import { Card, CardHeader } from '../../components/atoms/Card'
import { Input } from '../../components/atoms/Input'
import { SearchBar } from '../../components/molecules/SearchBar'

function amigoABackend(nombre: string, correo: string, index: number): Friend {
  return {
    id: `b${index}-${correo}`,
    name: nombre,
    handle: '@' + correo.split('@')[0].replace(/[^a-z0-9_]/gi, '').toLowerCase(),
    role: 'Amigo',
    tags: [],
    status: 'online',
    mutual: 0,
    recent: false,
    category: 'Red',
    connectedSince: '—',
  }
}

export function GestionPage() {
  const { user } = useAuth()
  const correo = user?.email ?? ''

  const [list, setList] = useState<Friend[]>([])
  const [query, setQuery] = useState('')
  const [nombreBusqueda, setNombreBusqueda] = useState('')
  const [resultados, setResultados] = useState<
    Array<{ nombre: string; correo: string; es_amigo: boolean }>
  >([])
  const [buscando, setBuscando] = useState(false)
  const [buscoAlgunaVez, setBuscoAlgunaVez] = useState(false)
  const [pendientes, setPendientes] = useState<SolicitudPendiente[]>([])
  const [enviadasCount, setEnviadasCount] = useState(0)
  const [pendingDelete, setPendingDelete] = useState<Friend | null>(null)
  const [formMsg, setFormMsg] = useState<string | null>(null)
  const [formError, setFormError] = useState<string | null>(null)
  const [cargando, setCargando] = useState(true)

  const refrescar = useCallback(async () => {
    if (!correo) return
    try {
      const [amigosRes, pendRes, envRes] = await Promise.all([
        api.amigos(correo),
        api.pendientes(correo),
        api.enviadas(correo),
      ])
      setList(amigosRes.amigos.map((a, i) => amigoABackend(a.nombre, a.correo, i)))
      setPendientes(pendRes.pendientes)
      setEnviadasCount(envRes.enviadas.length)
    } catch {
      /* backend apagado: se muestra lista vacia */
    } finally {
      setCargando(false)
    }
  }, [correo])

  useEffect(() => {
    void refrescar()
  }, [refrescar])

  const filtered = useMemo(() => {
    const q = query.trim().toLowerCase()
    if (!q) return list
    return list.filter(
      (friend) =>
        friend.name.toLowerCase().includes(q) || friend.handle.toLowerCase().includes(q),
    )
  }, [list, query])

  // Busca personas por nombre completo en el backend. El backend devuelve
  // a cada uno con es_amigo para saber si ponerle "Añadir amigo" o "Amigo".
  async function buscarPorNombre(event: React.FormEvent) {
    event.preventDefault()
    setFormMsg(null)
    setFormError(null)
    if (!nombreBusqueda.trim()) return
    setBuscando(true)
    try {
      const res = await api.usuarios(correo, nombreBusqueda.trim())
      setResultados(res.usuarios)
      setBuscoAlgunaVez(true)
    } catch (err) {
      setFormError(err instanceof Error ? err.message : 'No se pudo buscar')
    } finally {
      setBuscando(false)
    }
  }

  // Manda la solicitud y saca a la persona de los resultados para no
  // duplicarla en pantalla. Si ya habia algo pendiente, el backend avisa.
  async function enviarSolicitudA(destino: string) {
    setFormMsg(null)
    setFormError(null)
    try {
      await api.enviarSolicitud(correo, destino)
      setFormMsg(`Solicitud enviada a ${destino}`)
      setResultados((prev) => prev.filter((r) => r.correo !== destino))
      await refrescar()
    } catch (err) {
      setFormError(err instanceof Error ? err.message : 'No se pudo enviar')
    }
  }

  async function responder(de: string, accion: 'aceptar' | 'rechazar') {
    setFormError(null)
    try {
      if (accion === 'aceptar') await api.aceptar(de, correo)
      else await api.rechazar(de, correo)
      await refrescar()
    } catch (err) {
      setFormError(err instanceof Error ? err.message : 'No se pudo responder')
    }
  }

  function confirmDelete() {
    if (!pendingDelete) return
    setList((prev) => prev.filter((friend) => friend.id !== pendingDelete.id))
    setPendingDelete(null)
  }

  return (
    <div className="mx-auto flex w-full max-w-[680px] flex-col gap-4">
      <header className="flex flex-col gap-1 px-1 pt-2">
        <h1 className="text-headline text-ink">Gestionar red</h1>
        <p className="text-sm text-ink-subtle">
          {list.length} amigos · {pendientes.length} solicitudes · {enviadasCount} enviadas
        </p>
      </header>

      {formError ? (
        <p className="rounded-xl border border-danger/30 bg-danger/10 px-4 py-2.5 text-[13px] text-danger">
          {formError}
        </p>
      ) : null}
      {formMsg ? (
        <p className="rounded-xl border border-success/30 bg-success/10 px-4 py-2.5 text-[13px] text-success">
          {formMsg}
        </p>
      ) : null}

      {/* Buscar personas */}
      <Card className="overflow-hidden">
        <div className="px-5 pb-1 pt-5">
          <CardHeader
            title="Buscar personas"
            icon={<Search size={15} className="text-ink-subtle" />}
          />
          <form onSubmit={buscarPorNombre} className="mt-4 flex gap-2">
            <Input
              id="userId"
              required
              value={nombreBusqueda}
              onChange={(e) => setNombreBusqueda(e.target.value)}
              placeholder="Nombre completo, ej. Ana Lopez"
              icon={<Users size={15} />}
              className="flex-1"
            />
            <Button type="submit" disabled={buscando} className="shrink-0">
              {buscando ? '…' : 'Buscar'}
            </Button>
          </form>
        </div>
        {buscoAlgunaVez ? (
          resultados.length === 0 ? (
            <p className="px-5 py-4 text-[13px] text-ink-tertiary">Sin coincidencias.</p>
          ) : (
            <ul className="mt-3 divide-y divide-hairline border-t border-hairline">
              {resultados.map((r) => (
                <li key={r.correo} className="flex items-center gap-3 px-5 py-3">
                  <Avatar name={r.nombre} size="sm" />
                  <div className="min-w-0 flex-1">
                    <div className="truncate text-sm font-semibold text-ink">{r.nombre}</div>
                    <div className="truncate font-mono text-[11px] text-ink-tertiary">
                      {r.correo}
                    </div>
                  </div>
                  {r.es_amigo ? (
                    <span className="flex h-8 shrink-0 items-center gap-1 rounded-lg bg-surface-3 px-3 text-[13px] font-semibold text-ink-tertiary">
                      <Check size={13} /> Amigo
                    </span>
                  ) : (
                    <button
                      type="button"
                      onClick={() => void enviarSolicitudA(r.correo)}
                      className="h-8 shrink-0 rounded-lg bg-primary px-4 text-[13px] font-semibold text-white transition-colors hover:bg-primary-hover"
                    >
                      Añadir amigo
                    </button>
                  )}
                </li>
              ))}
            </ul>
          )
        ) : null}
      </Card>

      {/* Solicitudes */}
      <Card className="overflow-hidden">
        <div className="px-5 pt-5">
          <CardHeader
            title={`Solicitudes (${pendientes.length})`}
            icon={<UserMinus size={15} className="text-ink-subtle" />}
          />
        </div>
        {pendientes.length === 0 ? (
          <p className="px-5 py-4 text-[13px] text-ink-tertiary">
            {cargando ? 'Cargando…' : 'Nadie quiere conectar por ahora.'}
          </p>
        ) : (
          <ul className="mt-3 divide-y divide-hairline border-t border-hairline">
            {pendientes.map((s) => (
              <li key={s.de} className="flex items-center gap-3 px-5 py-3">
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
      </Card>

      {/* Amigos */}
      <Card className="overflow-hidden">
        <div className="px-5 pt-5">
          <CardHeader
            title={`Amigos (${list.length})`}
            icon={<Users size={15} className="text-ink-subtle" />}
          />
          <div className="mt-3">
            <SearchBar value={query} onChange={setQuery} placeholder="Buscar amigos…" />
          </div>
        </div>
        {filtered.length === 0 ? (
          <p className="px-5 py-4 text-[13px] text-ink-tertiary">
            {cargando ? 'Cargando…' : 'Sin amigos todavía.'}
          </p>
        ) : (
          <ul className="mt-3 divide-y divide-hairline border-t border-hairline">
            <AnimatePresence initial={false}>
              {filtered.map((friend) => (
                <motion.li
                  key={friend.id}
                  layout
                  initial={{ opacity: 0 }}
                  animate={{ opacity: 1 }}
                  exit={{ opacity: 0, height: 0 }}
                  transition={{ duration: 0.2 }}
                  className="flex items-center gap-3 overflow-hidden px-5 py-3"
                >
                  <Avatar name={friend.name} size="sm" online />
                  <div className="min-w-0 flex-1">
                    <div className="truncate text-sm font-semibold text-ink">{friend.name}</div>
                    <div className="truncate font-mono text-[11px] text-ink-tertiary">
                      {friend.handle}
                    </div>
                  </div>
                  <button
                    type="button"
                    onClick={() => setPendingDelete(friend)}
                    className="h-8 shrink-0 rounded-lg bg-surface-3 px-4 text-[13px] font-semibold text-ink transition-colors hover:bg-surface-4"
                  >
                    Eliminar
                  </button>
                </motion.li>
              ))}
            </AnimatePresence>
          </ul>
        )}
      </Card>

      {/* Modal de confirmación */}
      <AnimatePresence>
        {pendingDelete ? (
          <motion.div
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            className="fixed inset-0 z-50 grid place-items-center bg-black/70 px-5 backdrop-blur-sm"
            onClick={() => setPendingDelete(null)}
            role="presentation"
          >
            <motion.div
              initial={{ scale: 0.96, opacity: 0, y: 8 }}
              animate={{ scale: 1, opacity: 1, y: 0 }}
              exit={{ scale: 0.97, opacity: 0, y: 6 }}
              transition={{ duration: 0.22, ease: [0.16, 1, 0.3, 1] }}
              onClick={(event) => event.stopPropagation()}
              className="w-full max-w-sm rounded-2xl border border-hairline bg-surface-1 p-6 text-center"
              role="dialog"
              aria-modal="true"
            >
              <div className="mx-auto w-fit">
                <Avatar name={pendingDelete.name} size="lg" />
              </div>
              <h3 className="mt-3 text-base font-semibold text-ink">
                ¿Eliminar a {pendingDelete.name}?
              </h3>
              <p className="mt-1 text-[13px] leading-relaxed text-ink-subtle">
                Se revocan los canales compartidos con este contacto. No se puede deshacer.
              </p>
              <div className="mt-5 flex gap-2">
                <Button
                  variant="secondary"
                  type="button"
                  className="flex-1"
                  onClick={() => setPendingDelete(null)}
                >
                  Cancelar
                </Button>
                <button
                  type="button"
                  onClick={confirmDelete}
                  className="inline-flex h-9 flex-1 items-center justify-center gap-2 rounded-lg bg-danger px-3.5 text-sm font-semibold text-white transition-colors hover:opacity-90"
                >
                  <Trash2 size={14} />
                  Eliminar
                </button>
              </div>
            </motion.div>
          </motion.div>
        ) : null}
      </AnimatePresence>
    </div>
  )
}
