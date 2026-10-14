import { cn } from '../../lib/cn'

type Size = 'xs' | 'sm' | 'md' | 'lg'

const sizes: Record<Size, string> = {
  xs: 'size-7 text-[10px]',
  sm: 'size-9 text-xs',
  md: 'size-11 text-sm',
  lg: 'size-16 text-base',
}

interface AvatarProps {
  name: string
  size?: Size
  online?: boolean
  className?: string
}

function initials(name: string): string {
  return name
    .split(' ')
    .filter(Boolean)
    .slice(0, 2)
    .map((part) => part[0]?.toUpperCase())
    .join('')
}

export function Avatar({ name, size = 'md', online, className }: AvatarProps) {
  return (
    <span className={cn('relative inline-flex shrink-0', className)}>
      <span
        className={cn(
          'grid place-items-center rounded-full border border-hairline bg-surface-2 font-medium tracking-tight text-ink-muted',
          sizes[size],
        )}
      >
        {initials(name)}
      </span>
      {online !== undefined ? (
        <span
          className={cn(
            'absolute bottom-0 right-0 block rounded-full border-2 border-surface-1',
            online ? 'bg-success' : 'bg-ink-tertiary',
            size === 'lg' ? 'size-3.5' : 'size-2.5',
          )}
        />
      ) : null}
    </span>
  )
}
