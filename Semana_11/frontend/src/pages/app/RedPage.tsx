import {
  Crosshair,
  Expand,
  Gauge,
  Layers,
  Maximize2,
  Minus,
  Plus,
  Radio,
  Waypoints,
} from 'lucide-react'
import { useEffect, useMemo, useState } from 'react'
import type { ReactNode } from 'react'
import { useAuth } from '../../lib/auth'
import { api } from '../../lib/api'
import type { RedUsuario } from '../../lib/api'
import type { GraphNode } from '../../lib/types'
import { Badge } from '../../components/atoms/Badge'
import { Card, CardHeader } from '../../components/atoms/Card'
import { Avatar } from '../../components/atoms/Avatar'
import { PageHeader } from '../../components/molecules/PageHeader'
import { RedAmigosViewer } from '../../components/organisms/app/RedAmigosViewer'
import { TopologyGraph } from '../../components/organisms/app/TopologyGraph'
import { cn } from '../../lib/cn'

/* Reparte los nodos en circulos para que el lienzo se entienda de un
   vistazo: yo al centro, mis amigos en el anillo de adentro y los amigos
   en comun en el de afuera. Las posiciones se calculan aqui, pero las
   amistades (quien es amigo de quien) las manda el backend. */
function construirNodos(red: RedUsuario): GraphNode[] {
  const nodos: GraphNode[] = []
  if (red.yo) {
    nodos.push({
      id: red.yo.correo,
      label: red.yo.nombre,
      x: 50,
      y: 50,
      r: 9,
      status: 'online',
      group: 'yo',
    })
  }
  const anillo = (
    total: number,
    i: number,
    radio: number,
    desfase: number,
  ): [number, number] => {
    if (total === 1) return [50 + radio * Math.cos(desfase), 50 + radio * Math.sin(desfase)]
    // Angulo dorado (~137.5°): reparte sin dejar nodos diametralmente
    // opuestos, asi las aristas no se enciman en una sola linea.
    const ang = desfase + i * 2.399963
    return [50 + radio * Math.cos(ang), 50 + radio * Math.sin(ang)]
  }
  red.amigos.forEach((a, i) => {
    const [x, y] = anillo(red.amigos.length, i, 26, -Math.PI / 2)
    nodos.push({ id: a.correo, label: a.nombre, x, y, r: 6, status: 'online', group: 'amigos' })
  })
  red.amigos_en_comun.forEach((a, i) => {
    const [x, y] = anillo(red.amigos_en_comun.length, i, 40, -Math.PI / 2 + 0.9)
    nodos.push({ id: a.correo, label: a.nombre, x, y, r: 4, status: 'online', group: 'comunidad' })
  })
  return nodos
}

export function RedPage() {
  const { user } = useAuth()
  const correo = user?.email ?? ''
  const [red, setRed] = useState<RedUsuario | null>(null)
  const [cargando, setCargando] = useState(true)
  const [error, setError] = useState<string | null>(null)
  const [zoom, setZoom] = useState(1)
  const [resonance, setResonance] = useState(true)
  const [expanded, setExpanded] = useState(false)
  const [selectedId, setSelectedId] = useState('')

  // Al entrar se descarga la red una sola vez. El JSON trae nodos, aristas
  // y el DOT; con eso se arman el lienzo interactivo y la telemetria.
  useEffect(() => {
    if (!correo) return
    setCargando(true)
    api
      .red(correo)
      .then((data) => {
        setRed(data)
        setSelectedId((prev) => prev || data.yo?.correo || '')
        setError(null)
      })
      .catch((err: unknown) =>
        setError(err instanceof Error ? err.message : 'No se pudo cargar la red'),
      )
      .finally(() => setCargando(false))
  }, [correo])

  const nodos = useMemo(() => (red ? construirNodos(red) : []), [red])
  const aristas: Array<[string, string]> = useMemo(
    () => red?.aristas.map(([a, b]) => [a, b] as [string, string]) ?? [],
    [red],
  )

  const nombres = useMemo(() => {
    const mapa = new Map<string, { nombre: string; tipo: string }>()
    if (red?.yo) mapa.set(red.yo.correo, { nombre: red.yo.nombre, tipo: 'Tú' })
    red?.amigos.forEach((a) => mapa.set(a.correo, { nombre: a.nombre, tipo: 'Amigo' }))
    red?.amigos_en_comun.forEach((a) =>
      mapa.set(a.correo, { nombre: a.nombre, tipo: 'En común' }),
    )
    return mapa
  }, [red])

  const selected = nodos.find((node) => node.id === selectedId) ?? nodos[0]
  const selectedInfo = selected ? nombres.get(selected.id) : undefined

  // Vecinos del nodo seleccionado: se sacan de las aristas que mando el
  // backend (si la arista toca al seleccionado, el otro extremo es vecino).
  const vecinos = useMemo(() => {
    if (!selected) return [] as GraphNode[]
    const ids = aristas.flatMap(([a, b]) =>
      a === selected.id ? [b] : b === selected.id ? [a] : [],
    )
    return ids
      .map((id) => nodos.find((node) => node.id === id))
      .filter((node): node is GraphNode => node !== undefined && node.id !== selectedId)
  }, [aristas, nodos, selected, selectedId])

  const conectaCon = useMemo(() => {
    if (!selected || selected.group !== 'comunidad') return [] as string[]
    return red?.amigos_en_comun.find((a) => a.correo === selected.id)?.conecta_con ?? []
  }, [red, selected])

  const totalNodos = nodos.length
  const telemetry = [
    { label: 'Nodos en vista', value: String(totalNodos), detail: 'tu + red' },
    { label: 'Amigos directos', value: String(red?.amigos.length ?? 0), detail: 'aceptados' },
    { label: 'En común', value: String(red?.amigos_en_comun.length ?? 0), detail: '2.º nivel' },
    { label: 'Aristas', value: String(aristas.length), detail: 'no dirigidas' },
  ]

  return (
    <div className="flex flex-col gap-6">
      <PageHeader
        breadcrumbs={[{ label: 'FaceJens' }, { label: 'Topología social' }, { label: 'Malla' }]}
        eyebrow="Topología social"
        title="Malla de conexiones"
        description="Tu red real: nodos posicionados desde el grafo no dirigido."
        chips={
          <>
            <Badge tone="success">
              <span className="size-1.5 animate-pulse rounded-full bg-success" />
              {red ? `${totalNodos} nodos` : 'Sin datos'}
            </Badge>
            <Badge tone="neutral">{aristas.length} aristas</Badge>
          </>
        }
      />

      {cargando ? (
        <Card className="p-10 text-center">
          <p className="text-sm text-ink-muted">Cargando tu red…</p>
        </Card>
      ) : error || !red || !selected ? (
        <Card className="p-10 text-center">
          <p className="text-sm text-ink-muted">
            {error ?? 'Sin datos. Registra usuarios y crea amistades primero.'}
          </p>
        </Card>
      ) : (
        <div className="grid grid-cols-1 items-start gap-6 xl:grid-cols-12">
          {/* Lienzo del grafo */}
          <section
            className={cn(
              'relative overflow-hidden rounded-xl border border-hairline bg-surface-1',
              expanded ? 'xl:col-span-12' : 'xl:col-span-8',
            )}
          >
            <div className="flex items-center justify-between border-b border-hairline px-5 py-3">
              <div className="flex items-center gap-2">
                <Waypoints size={15} className="text-accent" />
                <span className="text-sm font-semibold text-ink">Lienzo interactivo</span>
              </div>
              <span className="font-mono text-xs text-ink-tertiary">
                zoom {Math.round(zoom * 100)}%
              </span>
            </div>

            <div className="relative h-[520px] overflow-hidden bg-canvas sm:h-[580px]">
              <div
                className="absolute inset-0 p-4 transition-transform duration-300 ease-out"
                style={{ transform: `scale(${zoom})` }}
              >
                <TopologyGraph
                  nodes={nodos}
                  edges={aristas}
                  resonance={resonance}
                  selectedId={selected.id}
                  onSelect={setSelectedId}
                />
              </div>

              {/* Barra de herramientas flotante */}
              <div className="absolute bottom-4 left-1/2 flex -translate-x-1/2 items-center gap-1 rounded-full border border-hairline bg-surface-1/95 px-2 py-1.5 backdrop-blur-md">
                <ToolButton
                  label="Alejar"
                  onClick={() => setZoom((z) => Math.max(0.7, +(z - 0.15).toFixed(2)))}
                >
                  <Minus size={14} />
                </ToolButton>
                <ToolButton
                  label="Acercar"
                  onClick={() => setZoom((z) => Math.min(1.6, +(z + 0.15).toFixed(2)))}
                >
                  <Plus size={14} />
                </ToolButton>
                <span className="mx-1 h-4 w-px bg-hairline" />
                <ToolButton label="Recentrar" onClick={() => setZoom(1)}>
                  <Crosshair size={14} />
                </ToolButton>
                <ToolButton
                  label="Modo resonancia"
                  onClick={() => setResonance((v) => !v)}
                  active={resonance}
                >
                  <Radio size={14} />
                </ToolButton>
                <ToolButton
                  label="Vista completa"
                  onClick={() => setExpanded((v) => !v)}
                  active={expanded}
                >
                  {expanded ? <Expand size={14} /> : <Maximize2 size={14} />}
                </ToolButton>
              </div>

              <div className="absolute left-4 top-4 flex flex-col gap-1.5 text-[11px] text-ink-tertiary">
                <span className="flex items-center gap-1.5">
                  <span className="size-2 rounded-full bg-[#e2685c]" /> Tú
                </span>
                <span className="flex items-center gap-1.5">
                  <span className="size-2 rounded-full bg-[#d1493d]" /> Amigos
                </span>
                <span className="flex items-center gap-1.5">
                  <span className="size-2 rounded-full bg-[#9aa0a6]" /> En común
                </span>
              </div>
            </div>
          </section>

          {/* Telemetría */}
          <aside className="flex flex-col gap-4 xl:col-span-4">
            <Card className="p-5">
              <CardHeader
                title="Telemetría de red"
                icon={<Gauge size={15} className="text-ink-subtle" />}
                action={<Badge tone="success">en vivo</Badge>}
              />
              <div className="mt-4 grid grid-cols-2 gap-2">
                {telemetry.map((item) => (
                  <div
                    key={item.label}
                    className="flex flex-col gap-1 rounded-lg border border-hairline bg-canvas px-3 py-2.5"
                  >
                    <span className="text-[11px] text-ink-subtle">{item.label}</span>
                    <span className="text-base font-semibold tracking-tight text-ink">
                      {item.value}
                    </span>
                    <span className="text-[10px] text-ink-tertiary">{item.detail}</span>
                  </div>
                ))}
              </div>
            </Card>

            <Card className="p-5">
              <CardHeader
                title="Detalle de nodo"
                icon={<Layers size={15} className="text-ink-subtle" />}
                action={<Badge tone="accent">{selectedInfo?.tipo ?? selected.group}</Badge>}
              />
              <div className="mt-4 flex items-center gap-3">
                <Avatar name={selected.label} size="md" online />
                <div className="min-w-0">
                  <div className="truncate text-[15px] font-semibold text-ink">
                    {selected.label}
                  </div>
                  <div className="truncate font-mono text-xs text-ink-tertiary">
                    {selected.id}
                  </div>
                </div>
              </div>

              <dl className="mt-4 grid grid-cols-2 gap-2 text-xs">
                <div className="rounded-lg bg-canvas px-3 py-2">
                  <dt className="text-ink-tertiary">Conexiones en vista</dt>
                  <dd className="font-medium text-ink">{vecinos.length}</dd>
                </div>
                <div className="rounded-lg bg-canvas px-3 py-2">
                  <dt className="text-ink-tertiary">Tipo</dt>
                  <dd className="font-medium text-ink">{selectedInfo?.tipo ?? '—'}</dd>
                </div>
              </dl>

              {conectaCon.length > 0 ? (
                <p className="mt-3 text-xs leading-relaxed text-ink-subtle">
                  Conecta a través de:{' '}
                  {conectaCon
                    .map((c) => nombres.get(c)?.nombre ?? c)
                    .join(', ')}
                </p>
              ) : null}

              {vecinos.length > 0 ? (
                <div className="mt-4 flex items-center gap-2">
                  <span className="text-[11px] text-ink-tertiary">Vecinos:</span>
                  <div className="flex -space-x-2">
                    {vecinos.slice(0, 4).map((node) => (
                      <Avatar key={node.id} name={node.label} size="xs" />
                    ))}
                  </div>
                </div>
              ) : null}
            </Card>
          </aside>
        </div>
      )}

      <RedAmigosViewer correo={correo} />
    </div>
  )
}

interface ToolButtonProps {
  label: string
  onClick: () => void
  active?: boolean
  children: ReactNode
}

function ToolButton({ label, onClick, active = false, children }: ToolButtonProps) {
  return (
    <button
      type="button"
      onClick={onClick}
      title={label}
      aria-label={label}
      className={cn(
        'grid size-8 place-items-center rounded-full transition-colors',
        active
          ? 'bg-primary text-white'
          : 'text-ink-subtle hover:bg-surface-2 hover:text-ink',
      )}
    >
      {children}
    </button>
  )
}
