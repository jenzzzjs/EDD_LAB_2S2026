import type { ReactNode } from 'react'
import { cn } from '../../lib/cn'

interface FormFieldProps {
  label: string
  hint?: string
  htmlFor?: string
  error?: string
  children: ReactNode
  className?: string
}

export function FormField({ label, hint, htmlFor, error, children, className }: FormFieldProps) {
  return (
    <div className={cn('flex flex-col gap-1.5', className)}>
      <div className="flex items-center justify-between gap-3">
        <label htmlFor={htmlFor} className="text-[13px] font-medium text-ink-muted">
          {label}
        </label>
        {hint || error ? (
          <span className={cn('text-xs', error ? 'text-danger' : 'text-ink-tertiary')}>
            {error ?? hint}
          </span>
        ) : null}
      </div>
      {children}
    </div>
  )
}
