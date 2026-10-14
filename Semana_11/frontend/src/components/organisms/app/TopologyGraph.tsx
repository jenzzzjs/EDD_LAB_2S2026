import { motion } from 'framer-motion'
import type { GraphNode } from '../../../lib/types'

interface TopologyGraphProps {
  nodes: GraphNode[]
  edges: Array<[string, string]>
  compact?: boolean
  showLabels?: boolean
  resonance?: boolean
  selectedId?: string | null
  onSelect?: (id: string) => void
  className?: string
}

const ONLINE = '#d1493d'
const COMUN = '#9aa0a6'
const OFFLINE = '#4a4142'
const SELF = '#e2685c'
const EDGE = '#3d2f32'
const EDGE_ACTIVE = '#e2685c'

function nodeColor(node: GraphNode): string {
  if (node.group === 'yo') return SELF
  if (node.group === 'comunidad') return COMUN
  return node.status === 'online' ? ONLINE : OFFLINE
}

export function TopologyGraph({
  nodes,
  edges,
  compact = false,
  showLabels = true,
  resonance = false,
  selectedId = null,
  onSelect,
  className = '',
}: TopologyGraphProps) {
  const byId = new Map(nodes.map((node) => [node.id, node]))
  const labels = showLabels && !compact

  // Arista curva (cuadrática) en vez de recta: el punto de control se empuja
  // hacia afuera del centro para que las aristas se abran en abanico y no
  // se encimen. `curve` mayor = arco más abierto (anillo-anillo).
  const edgePath = (
    ax: number,
    ay: number,
    bx: number,
    by: number,
    curve: number,
  ): string => {
    const mx = (ax + bx) / 2
    const my = (ay + by) / 2
    const dx = bx - ax
    const dy = by - ay
    const len = Math.hypot(dx, dy) || 1
    let nx = -dy / len
    let ny = dx / len
    if (nx * (mx - 50) + ny * (my - 50) < 0) {
      nx = -nx
      ny = -ny
    }
    return `M ${ax} ${ay} Q ${mx + nx * curve} ${my + ny * curve} ${bx} ${by}`
  }

  return (
    <svg
      viewBox="0 0 100 100"
      preserveAspectRatio="xMidYMid meet"
      className={`h-full w-full ${className}`}
      role="img"
      aria-label="Topología de la red social"
    >
      <defs>
        <radialGradient id="nodeGlow">
          <stop offset="0%" stopColor="#e2685c" stopOpacity="0.5" />
          <stop offset="100%" stopColor="#e2685c" stopOpacity="0" />
        </radialGradient>
      </defs>

      {/* Aristas curvas: cada una se abre hacia afuera */}
      {edges.map(([from, to], index) => {
        const a = byId.get(from)
        const b = byId.get(to)
        if (!a || !b) return null
        const active =
          selectedId !== null && (from === selectedId || to === selectedId)
        const tocaHub = a.group === 'yo' || b.group === 'yo'
        return (
          <motion.path
            key={`${from}-${to}`}
            d={edgePath(a.x, a.y, b.x, b.y, tocaHub ? 4.5 : 10)}
            fill="none"
            stroke={active ? EDGE_ACTIVE : EDGE}
            strokeWidth={active ? 0.5 : 0.35}
            strokeDasharray={resonance ? '2 2' : undefined}
            initial={{ pathLength: 0, opacity: 0 }}
            animate={{
              pathLength: 1,
              opacity: 1,
              strokeDashoffset: resonance ? [0, -8] : 0,
            }}
            transition={{
              pathLength: { delay: 0.1 + index * 0.03, duration: 0.6 },
              opacity: { delay: 0.1 + index * 0.03, duration: 0.4 },
              strokeDashoffset: { duration: 1.6, repeat: resonance ? Infinity : 0, ease: 'linear' },
            }}
          />
        )
      })}

      {/* Halo del nodo central */}
      <circle cx={50} cy={50} r={16} fill="url(#nodeGlow)" />

      {/* Nodos */}
      {nodes.map((node, index) => {
        const isSelf = node.group === 'yo'
        const isSelected = selectedId === node.id
        return (
          <motion.g
            key={node.id}
            initial={{ opacity: 0, scale: 0.4 }}
            animate={{ opacity: 1, scale: 1 }}
            transition={{ delay: 0.2 + index * 0.05, type: 'spring', stiffness: 260, damping: 20 }}
            style={{ transformBox: 'fill-box', transformOrigin: 'center' }}
            onClick={onSelect ? () => onSelect(node.id) : undefined}
            className={onSelect ? 'cursor-pointer' : undefined}
          >
            {isSelf ? (
              <motion.circle
                cx={node.x}
                cy={node.y}
                r={node.r}
                fill="none"
                stroke={SELF}
                strokeWidth={0.5}
                animate={{ r: [node.r, node.r + 8], opacity: [0.55, 0] }}
                transition={{ duration: 2.4, repeat: Infinity, ease: 'easeOut' }}
              />
            ) : null}

            {isSelected ? (
              <circle
                cx={node.x}
                cy={node.y}
                r={node.r + 3}
                fill="none"
                stroke={SELF}
                strokeWidth={0.5}
                strokeDasharray="1.5 1.5"
              />
            ) : null}

            <circle
              cx={node.x}
              cy={node.y}
              r={node.r}
              fill={nodeColor(node)}
              stroke={isSelf ? '#f7f3f3' : 'transparent'}
              strokeWidth={isSelf ? 0.6 : 0}
            />

            {node.status === 'online' && !isSelf ? (
              <circle cx={node.x} cy={node.y} r={node.r + 1.6} fill="none" stroke={ONLINE} strokeOpacity={0.3} strokeWidth={0.3} />
            ) : null}

            {labels ? (
              <text
                x={node.x}
                y={node.y + node.r + 4}
                textAnchor="middle"
                fontSize="3"
                fill={isSelf ? '#f7f3f3' : '#9b9092'}
                fontFamily="Inter, sans-serif"
              >
                {node.label}
              </text>
            ) : null}
          </motion.g>
        )
      })}
    </svg>
  )
}
