import { motion } from 'framer-motion'
import { cn } from '../../lib/cn'

export interface Segment {
  value: string
  label: string
}

interface SegmentedControlProps {
  items: Segment[]
  value: string
  onChange: (value: string) => void
  groupId?: string
  className?: string
  size?: 'sm' | 'md'
}

export function SegmentedControl({
  items,
  value,
  onChange,
  groupId = 'seg',
  className,
  size = 'md',
}: SegmentedControlProps) {
  return (
    <div
      className={cn(
        'inline-flex items-center gap-0.5 rounded-full border border-hairline bg-canvas p-1',
        className,
      )}
    >
      {items.map((item) => {
        const active = item.value === value
        return (
          <button
            key={item.value}
            type="button"
            onClick={() => onChange(item.value)}
            aria-pressed={active}
            className={cn(
              'relative rounded-full transition-colors',
              size === 'sm' ? 'px-3 py-1 text-xs' : 'px-3.5 py-1.5 text-[13px]',
              active ? 'text-ink' : 'text-ink-subtle hover:text-ink-muted',
            )}
          >
            {active ? (
              <motion.span
                layoutId={`segment-${groupId}`}
                className="absolute inset-0 rounded-full bg-surface-3 ring-1 ring-hairline-strong"
                transition={{ type: 'spring', stiffness: 420, damping: 34 }}
              />
            ) : null}
            <span className="relative z-10 font-medium">{item.label}</span>
          </button>
        )
      })}
    </div>
  )
}
