import { AnimatePresence, motion } from 'framer-motion'
import { AlertTriangle, CheckCheck, Send, Sparkles, X } from 'lucide-react'
import { createContext, useCallback, useContext, useMemo, useRef, useState } from 'react'
import type { ReactNode } from 'react'
import { cn } from '../../lib/cn'

export type ToastKind = 'success' | 'error' | 'info' | 'sending'

export interface ToastItem {
  id: number
  kind: ToastKind
  title: string
  message?: string
}

interface ToastContextValue {
  notify: (kind: ToastKind, title: string, message?: string) => void
  success: (title: string, message?: string) => void
  error: (title: string, message?: string) => void
  info: (title: string, message?: string) => void
  sending: (title: string, message?: string) => void
}

const ToastContext = createContext<ToastContextValue | null>(null)

export function useToast(): ToastContextValue {
  const ctx = useContext(ToastContext)
  if (!ctx) throw new Error('useToast debe usarse dentro de <ToastProvider>')
  return ctx
}

const DURATION_MS = 3600

const kindStyle: Record<ToastKind, { ring: string; glow: string; bar: string }> = {
  success: {
    ring: 'border-success/40 bg-success/15 text-success',
    glow: 'from-success/25 via-transparent to-transparent',
    bar: 'bg-success',
  },
  error: {
    ring: 'border-danger/40 bg-danger/15 text-danger',
    glow: 'from-danger/25 via-transparent to-transparent',
    bar: 'bg-danger',
  },
  info: {
    ring: 'border-accent/40 bg-accent/15 text-accent-strong',
    glow: 'from-accent/25 via-transparent to-transparent',
    bar: 'bg-accent',
  },
  sending: {
    ring: 'border-primary/50 bg-primary/15 text-accent-strong',
    glow: 'from-primary/30 via-transparent to-transparent',
    bar: 'bg-primary-focus',
  },
}

function ToastIcon({ kind }: { kind: ToastKind }) {
  if (kind === 'success') return <CheckCheck size={17} strokeWidth={2.4} />
  if (kind === 'error') return <AlertTriangle size={17} strokeWidth={2.4} />
  if (kind === 'sending') return <Send size={16} strokeWidth={2.4} />
  return <Sparkles size={17} strokeWidth={2.4} />
}

function ToastCard({
  toast,
  onClose,
}: {
  toast: ToastItem
  onClose: (id: number) => void
}) {
  const style = kindStyle[toast.kind]
  const isSending = toast.kind === 'sending'

  return (
    <motion.div
      layout
      role="status"
      aria-live="polite"
      initial={{ opacity: 0, y: -28, scale: 0.92, filter: 'blur(6px)' }}
      animate={{ opacity: 1, y: 0, scale: 1, filter: 'blur(0px)' }}
      exit={{ opacity: 0, y: -16, scale: 0.9, filter: 'blur(4px)' }}
      transition={{ type: 'spring', stiffness: 420, damping: 30, mass: 0.9 }}
      className="pointer-events-auto relative w-[min(360px,calc(100vw-2.5rem))] overflow-hidden rounded-2xl border border-hairline-strong bg-surface-1/95 shadow-[0_18px_50px_-12px_rgba(0,0,0,0.8)] backdrop-blur-xl"
    >
      {/* halo de color + barrido de brillo */}
      <span
        aria-hidden
        className={cn(
          'pointer-events-none absolute inset-0 bg-gradient-to-br',
          style.glow,
        )}
      />
      <motion.span
        aria-hidden
        initial={{ x: '-120%' }}
        animate={{ x: '240%' }}
        transition={{ duration: 1.4, ease: [0.16, 1, 0.3, 1], delay: 0.15 }}
        className="pointer-events-none absolute inset-y-0 w-1/3 -skew-x-12 bg-gradient-to-r from-transparent via-white/[0.09] to-transparent"
      />

      <div className="relative flex items-start gap-3 px-4 pb-3.5 pt-3.5">
        {/* icono con pop + anillo expansivo */}
        <span className="relative mt-0.5 grid shrink-0 place-items-center">
          {toast.kind === 'success' ? (
            <motion.span
              aria-hidden
              initial={{ opacity: 0.9, scale: 0.5 }}
              animate={{ opacity: 0, scale: 1.9 }}
              transition={{ duration: 0.7, ease: 'easeOut' }}
              className="absolute size-9 rounded-full border border-success/50"
            />
          ) : null}
          <motion.span
            initial={{ scale: 0, rotate: -40 }}
            animate={{ scale: 1, rotate: 0 }}
            transition={{ type: 'spring', stiffness: 550, damping: 17, delay: 0.06 }}
            className={cn(
              'grid size-9 place-items-center rounded-full border',
              style.ring,
            )}
          >
            {isSending ? (
              <motion.span
                animate={{ x: [0, 2.5, 0], y: [0, -2.5, 0], rotate: [0, -8, 0] }}
                transition={{ duration: 1.6, repeat: Infinity, ease: 'easeInOut' }}
                className="grid place-items-center"
              >
                <ToastIcon kind={toast.kind} />
              </motion.span>
            ) : (
              <ToastIcon kind={toast.kind} />
            )}
          </motion.span>
          {isSending ? (
            <motion.span
              aria-hidden
              animate={{ opacity: [0.4, 1, 0.4] }}
              transition={{ duration: 1.6, repeat: Infinity, ease: 'easeInOut' }}
              className="absolute -bottom-1 -right-1 size-2.5 rounded-full bg-primary-focus"
            />
          ) : null}
        </span>

        <div className="min-w-0 flex-1">
          <motion.p
            initial={{ opacity: 0, y: 6 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.08, duration: 0.25 }}
            className="truncate text-sm font-semibold text-ink"
          >
            {toast.title}
          </motion.p>
          {toast.message ? (
            <motion.p
              initial={{ opacity: 0, y: 6 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ delay: 0.14, duration: 0.25 }}
              className="mt-0.5 break-words text-[13px] leading-snug text-ink-subtle"
            >
              {toast.message}
            </motion.p>
          ) : null}
        </div>

        <button
          type="button"
          onClick={() => onClose(toast.id)}
          aria-label="Cerrar notificación"
          className="grid size-6 shrink-0 place-items-center rounded-md text-ink-tertiary transition-colors hover:bg-surface-3 hover:text-ink"
        >
          <X size={13} />
        </button>
      </div>

      {/* barra de vida */}
      <motion.div
        aria-hidden
        initial={{ scaleX: 1 }}
        animate={{ scaleX: 0 }}
        transition={{ duration: DURATION_MS / 1000, ease: 'linear' }}
        className={cn('relative h-[3px] w-full origin-left', style.bar)}
      />
    </motion.div>
  )
}

export function ToastProvider({ children }: { children: ReactNode }) {
  const [toasts, setToasts] = useState<ToastItem[]>([])
  const idRef = useRef(0)
  const timers = useRef(new Map<number, ReturnType<typeof setTimeout>>())

  const dismiss = useCallback((id: number) => {
    setToasts((prev) => prev.filter((t) => t.id !== id))
    const timer = timers.current.get(id)
    if (timer) {
      clearTimeout(timer)
      timers.current.delete(id)
    }
  }, [])

  const notify = useCallback(
    (kind: ToastKind, title: string, message?: string) => {
      idRef.current += 1
      const id = idRef.current
      setToasts((prev) => [...prev.slice(-3), { id, kind, title, message }])
      const timer = setTimeout(() => dismiss(id), DURATION_MS + 350)
      timers.current.set(id, timer)
    },
    [dismiss],
  )

  const value = useMemo<ToastContextValue>(
    () => ({
      notify,
      success: (title, message) => notify('success', title, message),
      error: (title, message) => notify('error', title, message),
      info: (title, message) => notify('info', title, message),
      sending: (title, message) => notify('sending', title, message),
    }),
    [notify],
  )

  return (
    <ToastContext.Provider value={value}>
      {children}
      {/* viewport fijo arriba: no empuja el layout, vive sobre el contenido */}
      <div
        aria-live="polite"
        className="pointer-events-none fixed inset-x-0 top-5 z-[100] flex flex-col items-center gap-2 px-5 sm:inset-x-auto sm:right-5 sm:items-end sm:px-0"
      >
        <AnimatePresence mode="popLayout">
          {toasts.map((toast) => (
            <ToastCard key={toast.id} toast={toast} onClose={dismiss} />
          ))}
        </AnimatePresence>
      </div>
    </ToastContext.Provider>
  )
}
