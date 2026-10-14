import { useState } from 'react'
import { Network } from 'lucide-react'
import { api } from '../../../lib/api'
import type { RedUsuario } from '../../../lib/api'
import { Card, CardHeader } from '../../atoms/Card'
import { Badge } from '../../atoms/Badge'

interface Props {
  correo: string
}

/* Boton "Ver red de amigos" + la imagen que genera el backend con Graphviz.
   Colores que manda el backend: yo = verde, amigos = azul,
   amigos en comun = gris. Este componente solo la pide y la muestra. */
export function RedAmigosViewer({ correo }: Props) {
  const [red, setRed] = useState<RedUsuario | null>(null)
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState<string | null>(null)
  const [visible, setVisible] = useState(false)

  async function verRed() {
    if (!correo) {
      setError('Inicia sesión para ver tu red')
      return
    }
    setLoading(true)
    setError(null)
    try {
      // Se le pide la red al backend y el JSON ya trae todo: listas, DOT
      // y el PNG en base64 listo para meterlo en un <img>.
      const data = await api.red(correo)
      setRed(data)
      setVisible(true)
    } catch (err) {
      setError(err instanceof Error ? err.message : 'No se pudo cargar la red')
    } finally {
      setLoading(false)
    }
  }

  return (
    <Card className="p-5">
      <CardHeader
        title="Red de amigos (Graphviz)"
        icon={<Network size={15} className="text-accent" />}
        action={
          <button
            type="button"
            onClick={verRed}
            disabled={loading}
            className="rounded-lg bg-primary px-3 py-1.5 text-xs font-medium text-white transition-colors hover:bg-primary-hover disabled:opacity-60"
          >
            {loading ? 'Generando…' : 'Ver red de amigos'}
          </button>
        }
      />

      <div className="mt-3 flex flex-wrap items-center gap-2 text-[11px] text-ink-subtle">
        <span className="flex items-center gap-1.5">
          <span className="size-2.5 rounded-full bg-[#22c55e]" /> Tú (verde)
        </span>
        <span className="flex items-center gap-1.5">
          <span className="size-2.5 rounded-full bg-[#2563eb]" /> Amigos (azul)
        </span>
        <span className="flex items-center gap-1.5">
          <span className="size-2.5 rounded-full bg-[#9ca3af]" /> Amigos en común (gris)
        </span>
      </div>

      {error ? (
        <p className="mt-3 rounded-lg border border-danger/30 bg-danger/10 px-3 py-2 text-[13px] text-danger">
          {error}
        </p>
      ) : null}

      {visible && red ? (
        <div className="mt-4 flex flex-col gap-3">
          <div className="flex flex-wrap gap-2">
            <Badge tone="success">{red.amigos.length} amigos</Badge>
            <Badge tone="neutral">{red.amigos_en_comun.length} en común</Badge>
          </div>
          {red.imagen_base64 ? (
            <img
              src={`data:image/png;base64,${red.imagen_base64}`}
              alt="Grafo no dirigido de la red de amigos"
              className="w-full rounded-lg border border-hairline bg-white"
            />
          ) : (
            <div className="flex flex-col gap-2">
              <p className="text-xs text-ink-subtle">
                {red.advertencia ?? 'Graphviz no disponible en el servidor.'}
              </p>
              <img
                src={api.redPngUrl(correo)}
                alt="Grafo no dirigido de la red de amigos"
                className="w-full rounded-lg border border-hairline bg-white"
              />
            </div>
          )}
          <details className="rounded-lg border border-hairline bg-canvas p-3">
            <summary className="cursor-pointer text-xs font-medium text-ink-muted">
              Ver DOT generado (grafo no dirigido)
            </summary>
            <pre className="mt-2 max-h-48 overflow-auto font-mono text-[11px] leading-relaxed text-ink-subtle">
              {red.dot}
            </pre>
          </details>
        </div>
      ) : null}
    </Card>
  )
}
