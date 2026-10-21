import { motion } from 'framer-motion'
import { TopologyGraph } from '../app/TopologyGraph'
import { graphEdges, graphNodes } from '../../../lib/data'

const GRAIN =
  "url(\"data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' width='160' height='160'><filter id='n'><feTurbulence type='fractalNoise' baseFrequency='0.9' numOctaves='2'/></filter><rect width='100%25' height='100%25' filter='url(%23n)' opacity='0.6'/></svg>\")"

/**
 * Arte del hero: un degradado rojo/negro que sostiene la malla viva.
 * Mismo patrón que la landing de MilAldeas: mensaje a la izquierda,
 * degradado + imagen a la derecha, fundido con máscaras.
 */
export function HeroVisual() {
  return (
    <div className="pointer-events-none absolute inset-0 overflow-hidden" aria-hidden="true">
      {/* Degradado rojo/negro ("la imagen" de fondo) */}
      <div
        className="absolute inset-y-0 right-0 w-full lg:w-[70%]"
        style={{
          background:
            'radial-gradient(80% 65% at 70% 28%, rgba(179,45,35,0.5) 0%, rgba(179,45,35,0.1) 45%, transparent 72%), radial-gradient(70% 60% at 85% 82%, rgba(107,28,23,0.85) 0%, rgba(74,21,18,0.35) 45%, transparent 70%), linear-gradient(115deg, #0a0708 0%, #1c0c0a 42%, #45140e 100%)',
          maskImage:
            'linear-gradient(90deg, transparent 0%, #000 30%, #000 100%), linear-gradient(180deg, #000 0%, #000 100%)',
          WebkitMaskImage:
            'linear-gradient(90deg, transparent 0%, #000 30%, #000 100%)',
        }}
      />

      {/* Malla viva sobre el degradado */}
      <div
        className="absolute inset-y-0 right-0 w-full pt-6 lg:w-[64%] lg:pb-4"
        style={{
          maskImage:
            'linear-gradient(90deg, transparent 0%, #000 26%, #000 100%), linear-gradient(180deg, #000 0%, #000 62%, transparent 100%)',
          WebkitMaskImage:
            'linear-gradient(90deg, transparent 0%, #000 26%, #000 100%)',
        }}
      >
        <TopologyGraph nodes={graphNodes} edges={graphEdges} resonance showLabels={false} />
      </div>

      {/* Orbes de luz pulsantes */}
      <motion.div
        animate={{ scale: [1, 1.28, 1], opacity: [0.45, 0.85, 0.45] }}
        transition={{ duration: 6, repeat: Infinity, ease: 'easeInOut' }}
        className="absolute right-[14%] top-[16%] size-[26rem] rounded-full bg-primary/25 blur-[120px]"
      />
      <motion.div
        animate={{ scale: [1, 1.18, 1], opacity: [0.35, 0.6, 0.35] }}
        transition={{ duration: 7.5, repeat: Infinity, ease: 'easeInOut', delay: 1.2 }}
        className="absolute bottom-[10%] right-[28%] size-[20rem] rounded-full bg-accent/20 blur-[110px]"
      />

      {/* Grano cinematográfico */}
      <div className="absolute inset-0 opacity-[0.16] mix-blend-overlay" style={{ backgroundImage: GRAIN }} />

      {/* Overlay de lectura sobre la izquierda */}
      <div
        className="absolute inset-0"
        style={{
          background:
            'linear-gradient(90deg, #0a0708 0%, rgba(10,7,8,0.94) 30%, rgba(10,7,8,0.55) 50%, rgba(10,7,8,0.28) 62%, transparent 78%)',
        }}
      />
    </div>
  )
}