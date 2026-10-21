import type { ReactNode } from 'react'
import { cn } from '../../lib/cn'

interface Breadcrumb {
  label: string
}

interface PageHeaderProps {
  eyebrow?: string
  title: string
  description?: string
  breadcrumbs?: Breadcrumb[]
  actions?: ReactNode
  chips?: ReactNode
  className?: string
}

export function PageHeader({
  eyebrow,
  title,
  description,
  breadcrumbs,
  actions,
  chips,
  className,
}: PageHeaderProps) {
  return (
    <header className={cn('flex flex-col gap-4', className)}>
      {breadcrumbs?.length ? (
        <nav className="flex items-center gap-2 text-xs text-ink-tertiary">
          {breadcrumbs.map((crumb, index) => (
            <span key={crumb.label} className="flex items-center gap-2">
              {index > 0 ? <span className="text-hairline-strong">/</span> : null}
              <span className={cn(index === breadcrumbs.length - 1 && 'text-accent')}>
                {crumb.label}
              </span>
            </span>
          ))}
        </nav>
      ) : null}

      <div className="flex flex-col gap-4 md:flex-row md:items-end md:justify-between">
        <div className="flex flex-col gap-2">
          {eyebrow ? <span className="text-eyebrow text-accent">{eyebrow}</span> : null}
          <h1 className="text-display-md text-ink">{title}</h1>
          {description ? (
            <p className="max-w-2xl text-sm leading-relaxed text-ink-subtle">{description}</p>
          ) : null}
          {chips ? <div className="flex flex-wrap items-center gap-2 pt-1">{chips}</div> : null}
        </div>
        {actions ? <div className="flex shrink-0 items-center gap-2">{actions}</div> : null}
      </div>
    </header>
  )
}
