import type { InputHTMLAttributes, ReactNode } from 'react'
import { cn } from '../../lib/cn'

interface InputProps extends InputHTMLAttributes<HTMLInputElement> {
  icon?: ReactNode
  trailing?: ReactNode
}

export function Input({ icon, trailing, className, ...props }: InputProps) {
  return (
    <div className="relative flex items-center">
      {icon ? (
        <span className="pointer-events-none absolute left-3 flex items-center text-ink-tertiary">{icon}</span>
      ) : null}
      <input
        className={cn(
          'h-10 w-full rounded-lg border border-hairline bg-canvas text-sm text-ink transition-colors',
          'placeholder:text-ink-tertiary hover:border-hairline-strong',
          'focus:border-primary-focus focus:outline-none focus:ring-2 focus:ring-primary-focus/40',
          icon ? 'pl-9' : 'pl-3',
          trailing ? 'pr-10' : 'pr-3',
          className,
        )}
        {...props}
      />
      {trailing ? <span className="absolute right-2 flex items-center">{trailing}</span> : null}
    </div>
  )
}
