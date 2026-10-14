import type { LucideIcon } from 'lucide-react'
import { cn } from '../../lib/cn'

interface StatTileProps {
  label: string
  value: string
  detail?: string
  icon?: LucideIcon
  className?: string
}

export function StatTile({ label, value, detail, icon: Icon, className }: StatTileProps) {
  return (
    <div
      className={cn(
        'flex flex-col gap-1 rounded-xl border border-hairline bg-surface-1 px-4 py-3.5',
        className,
      )}
    >
      <div className="flex items-center justify-between gap-2">
        <span className="text-xs font-medium text-ink-subtle">{label}</span>
        {Icon ? (
          <span className="grid size-6 place-items-center rounded-md bg-surface-2 text-ink-subtle">
            <Icon size={13} />
          </span>
        ) : null}
      </div>
      <span className="text-xl font-semibold tracking-[-0.02em] text-ink">{value}</span>
      {detail ? <span className="text-xs text-ink-tertiary">{detail}</span> : null}
    </div>
  )
}
