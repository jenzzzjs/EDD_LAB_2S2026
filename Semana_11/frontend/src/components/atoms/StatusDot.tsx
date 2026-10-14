import { cn } from '../../lib/cn'

interface StatusDotProps {
  online: boolean
  pulse?: boolean
  className?: string
}

export function StatusDot({ online, pulse = false, className }: StatusDotProps) {
  return (
    <span className={cn('relative flex size-2', className)}>
      {online && pulse ? (
        <span className="absolute inline-flex size-full animate-ping rounded-full bg-success opacity-60" />
      ) : null}
      <span
        className={cn('relative inline-flex size-2 rounded-full', online ? 'bg-success' : 'bg-ink-tertiary')}
      />
    </span>
  )
}
