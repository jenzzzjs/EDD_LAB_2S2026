import type { ReactNode } from 'react'
import { cn } from '../../lib/cn'

interface CardProps {
  className?: string
  hover?: boolean
  children: ReactNode
}

export function Card({ className, hover = false, children }: CardProps) {
  return (
    <div
      className={cn(
        'rounded-xl border border-hairline bg-surface-1',
        hover && 'transition-colors duration-200 hover:border-hairline-strong hover:bg-surface-2',
        className,
      )}
    >
      {children}
    </div>
  )
}

interface CardHeaderProps {
  title: ReactNode
  icon?: ReactNode
  action?: ReactNode
  className?: string
}

export function CardHeader({ title, icon, action, className }: CardHeaderProps) {
  return (
    <div className={cn('flex items-center justify-between gap-3', className)}>
      <div className="flex items-center gap-2 text-ink">
        {icon}
        <h2 className="text-sm font-semibold tracking-[-0.01em]">{title}</h2>
      </div>
      {action}
    </div>
  )
}
