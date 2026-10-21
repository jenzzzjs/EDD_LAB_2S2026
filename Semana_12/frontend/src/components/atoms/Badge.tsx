import type { ReactNode } from 'react'
import { cn } from '../../lib/cn'

type Tone = 'neutral' | 'accent' | 'success' | 'danger' | 'warning'

const tones: Record<Tone, string> = {
  neutral: 'border-hairline bg-surface-2 text-ink-muted',
  accent: 'border-primary/30 bg-primary/10 text-accent',
  success: 'border-success/30 bg-success/10 text-success',
  danger: 'border-danger/30 bg-danger/10 text-danger',
  warning: 'border-warning/30 bg-warning/10 text-warning',
}

interface BadgeProps {
  tone?: Tone
  className?: string
  children: ReactNode
}

export function Badge({ tone = 'neutral', className, children }: BadgeProps) {
  return (
    <span
      className={cn(
        'inline-flex items-center gap-1.5 rounded-full border px-2.5 py-0.5 text-xs font-medium leading-5',
        tones[tone],
        className,
      )}
    >
      {children}
    </span>
  )
}
