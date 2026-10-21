import { motion } from 'framer-motion'
import { BadgeCheck, MessageCircle, MoreHorizontal, Share2, WifiOff } from 'lucide-react'
import { cn } from '../../../lib/cn'
import type { Friend } from '../../../lib/types'
import { Avatar } from '../../atoms/Avatar'
import { Badge } from '../../atoms/Badge'

interface FriendCardProps {
  friend: Friend
  preview?: boolean
}

export function FriendCard({ friend, preview = false }: FriendCardProps) {
  const online = friend.status === 'online'

  if (preview) {
    return (
      <div className="flex items-center gap-2 rounded-lg border border-hairline bg-surface-1 px-2.5 py-2">
        <Avatar name={friend.name} size="xs" online={online} />
        <div className="min-w-0 flex-1">
          <div className="flex items-center gap-1">
            <span className="truncate text-[11px] font-medium text-ink">{friend.name}</span>
            {friend.verified ? <BadgeCheck size={10} className="shrink-0 text-accent" /> : null}
          </div>
          <div className="truncate text-[10px] text-ink-tertiary">{friend.role}</div>
        </div>
        <span
          className={cn(
            'size-1.5 shrink-0 rounded-full',
            online ? 'bg-success' : 'bg-ink-tertiary',
          )}
        />
      </div>
    )
  }

  return (
    <motion.article
      whileHover={{ y: -2 }}
      transition={{ type: 'spring', stiffness: 400, damping: 30 }}
      className={cn(
        'group flex flex-col rounded-xl border border-hairline bg-surface-1 p-5 transition-colors hover:border-hairline-strong',
        !online && 'opacity-75 hover:opacity-100',
      )}
    >
      <div className="flex items-start justify-between gap-3">
        <Avatar name={friend.name} size="lg" online={online} />
        <div className="flex items-center gap-2">
          <Badge tone={online ? 'success' : 'neutral'}>
            <span
              className={cn(
                'size-1.5 rounded-full',
                online ? 'animate-pulse bg-success' : 'bg-ink-tertiary',
              )}
            />
            {online ? 'En línea' : 'Desconectado'}
          </Badge>
          <button
            type="button"
            aria-label="Opciones"
            className="grid size-7 place-items-center rounded-md text-ink-tertiary transition-colors hover:bg-surface-2 hover:text-ink-muted"
          >
            <MoreHorizontal size={15} />
          </button>
        </div>
      </div>

      <div className="mt-4 flex flex-col gap-0.5">
        <h3 className="flex items-center gap-1.5 text-[15px] font-semibold tracking-[-0.01em] text-ink">
          {friend.name}
          {friend.verified ? <BadgeCheck size={14} className="text-accent" /> : null}
        </h3>
        <span className="font-mono text-xs text-ink-tertiary">{friend.handle}</span>
      </div>

      <div className="mt-3 flex flex-wrap gap-1.5">
        <span className="rounded-md border border-primary/25 bg-primary/10 px-2 py-0.5 text-[11px] font-medium text-accent">
          {friend.role}
        </span>
        {friend.tags.map((tag) => (
          <span
            key={tag}
            className="rounded-md border border-hairline bg-surface-2 px-2 py-0.5 text-[11px] text-ink-subtle"
          >
            {tag}
          </span>
        ))}
      </div>

      <div className="mt-3 flex items-center gap-2 rounded-lg bg-canvas px-3 py-2 text-xs text-ink-subtle">
        <span className="text-accent">{friend.mutual}</span> nodos en común con tu red
      </div>

      <div className="mt-4 flex items-center gap-2 border-t border-hairline pt-3">
        <button
          type="button"
          className={
            online
              ? 'flex h-9 flex-1 items-center justify-center gap-1.5 rounded-lg bg-primary text-[13px] font-medium text-white transition-colors hover:bg-primary-hover'
              : 'flex h-9 flex-1 items-center justify-center gap-1.5 rounded-lg border border-hairline bg-surface-2 text-[13px] font-medium text-ink-muted transition-colors hover:bg-surface-3'
          }
        >
          <MessageCircle size={14} />
          {online ? 'Enviar mensaje' : 'Dejar mensaje'}
        </button>
        <button
          type="button"
          aria-label="Compartir nodo"
          className="grid size-9 place-items-center rounded-lg border border-hairline text-ink-subtle transition-colors hover:bg-surface-2 hover:text-ink"
        >
          <Share2 size={14} />
        </button>
      </div>

      {!online ? (
        <span className="mt-2 flex items-center gap-1 text-[11px] text-ink-tertiary">
          <WifiOff size={11} /> Último ping hace 3 h
        </span>
      ) : null}
    </motion.article>
  )
}
