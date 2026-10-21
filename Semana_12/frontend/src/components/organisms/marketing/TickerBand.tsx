import { Radio } from 'lucide-react'
import { friends, graphNodes, suggestions } from '../../../lib/data'

const people = [
  friends[0].name,
  friends[1].name,
  friends[2].name,
  suggestions[0].name,
  friends[3].name,
  friends[4].name,
  suggestions[1].name,
  friends[5].name,
  suggestions[2].name,
  graphNodes[10].label,
  graphNodes[9].label,
  graphNodes[7].label,
  graphNodes[8].label,
]

export function TickerBand() {
  const track = [...people, ...people]
  return (
    <div className="relative overflow-hidden border-y border-hairline bg-surface-1/50">
      <div
        className="pointer-events-none absolute inset-y-0 left-0 w-16"
        style={{ background: 'linear-gradient(90deg, #0a0708 0%, transparent 100%)' }}
      />
      <div
        className="pointer-events-none absolute inset-y-0 right-0 w-16"
        style={{ background: 'linear-gradient(270deg, #0a0708 0%, transparent 100%)' }}
      />
      <div className="flex w-max animate-marquee items-center gap-10 py-3.5">
        {track.map((name, index) => (
          <span
            key={index}
            className="flex shrink-0 items-center gap-10 whitespace-nowrap text-[13px] font-medium tracking-[0.02em] text-ink-subtle"
          >
            <span className="flex items-center gap-2 transition-colors hover:text-ink">
              <span className="size-1.5 rounded-full bg-primary-focus" />
              {name}
            </span>
            <Radio size={9} className="text-primary" />
          </span>
        ))}
      </div>
    </div>
  )
}