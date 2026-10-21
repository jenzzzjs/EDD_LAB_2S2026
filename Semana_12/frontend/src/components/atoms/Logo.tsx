import { Network } from 'lucide-react'
import { Link } from 'react-router-dom'
import { cn } from '../../lib/cn'

interface LogoProps {
  size?: 'sm' | 'md'
  to?: string
  className?: string
  tagline?: string
  onClick?: () => void
}

const sizes = {
  sm: { box: 'size-7', icon: 15, text: 'text-sm' },
  md: { box: 'size-8', icon: 17, text: 'text-base' },
}

export function Logo({ size = 'md', to = '/', className, tagline, onClick }: LogoProps) {
  const s = sizes[size]
  return (
    <Link to={to} onClick={onClick} className={cn('group flex items-center gap-2.5', className)}>
      <span
        className={cn(
          'grid place-items-center rounded-lg bg-primary text-white transition-colors group-hover:bg-primary-hover',
          s.box,
        )}
      >
        <Network size={s.icon} strokeWidth={2.2} />
      </span>
<span className="flex flex-col leading-none">
        <span className={cn('font-semibold tracking-[-0.02em] text-ink', s.text)}>
          Face<span className="text-accent">Jens</span>
        </span>
        {tagline ? (
          <span className="mt-1 text-[10px] font-medium uppercase tracking-[0.14em] text-ink-tertiary">
            {tagline}
          </span>
        ) : null}
      </span>
    </Link>
  )
}
