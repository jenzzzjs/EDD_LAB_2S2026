import { Home, LogOut, Menu, MessageCircle, Network, Users, X } from 'lucide-react'
import { motion } from 'framer-motion'
import { useState } from 'react'
import { NavLink, useNavigate } from 'react-router-dom'
import { cn } from '../../../lib/cn'
import { useAuth } from '../../../lib/auth'
import { Avatar } from '../../atoms/Avatar'
import { Logo } from '../../atoms/Logo'

const items = [
  { to: '/app/inicio', label: 'Inicio', icon: Home },
  { to: '/app/red', label: 'Red', icon: Network },
  { to: '/app/chats', label: 'Chats', icon: MessageCircle },
  { to: '/app/gestion', label: 'Gestión', icon: Users },
]

export function AppHeader() {
  const { user, logout } = useAuth()
  const navigate = useNavigate()
  const [open, setOpen] = useState(false)

  function handleLogout() {
    logout()
    navigate('/')
  }

  return (
    <header className="sticky top-0 z-40 border-b border-hairline bg-canvas/85 backdrop-blur-md">
      <div className="mx-auto flex h-14 max-w-[1280px] items-center justify-between gap-4 px-5 lg:px-8">
        <div className="flex items-center gap-6">
          <Logo size="sm" to="/app/inicio" />

          <nav className="hidden items-center gap-1 md:flex">
            {items.map((item) => (
              <NavLink
                key={item.to}
                to={item.to}
                className={({ isActive }) =>
                  cn(
                    'relative flex items-center gap-1.5 rounded-lg px-3 py-1.5 text-[13px] font-medium transition-colors',
                    isActive
                      ? 'text-ink'
                      : 'text-ink-subtle hover:bg-surface-1 hover:text-ink-muted',
                  )
                }
              >
                {({ isActive }) => (
                  <>
                    {isActive ? (
                      <motion.span
                        layoutId="app-nav-active"
                        className="absolute inset-0 -z-10 rounded-lg border border-hairline bg-surface-2"
                        transition={{ type: 'spring', stiffness: 450, damping: 36 }}
                      />
                    ) : null}
                    <item.icon size={14} />
                    {item.label}
                  </>
                )}
              </NavLink>
            ))}
          </nav>
        </div>

        <div className="flex items-center gap-3">
          <span className="hidden items-center gap-2 rounded-full border border-hairline bg-surface-1 px-2.5 py-1 text-xs text-ink-muted lg:flex">
            <span className="size-1.5 rounded-full bg-success" />
            Malla en línea
          </span>

          <div className="hidden items-center gap-2.5 sm:flex">
            <div className="flex flex-col leading-tight text-right">
              <span className="text-[13px] font-medium text-ink">{user?.name ?? 'Invitado'}</span>
              <span className="text-[11px] text-ink-tertiary">{user?.handle ?? '@nexus'}</span>
            </div>
            <Avatar name={user?.name ?? 'jens'} size="sm" online />
          </div>

          <button
            type="button"
            onClick={handleLogout}
            aria-label="Cerrar sesión"
            className="grid size-8 place-items-center rounded-lg border border-hairline text-ink-subtle transition-colors hover:bg-surface-2 hover:text-ink"
          >
            <LogOut size={14} />
          </button>

          <button
            type="button"
            onClick={() => setOpen((v) => !v)}
            aria-label="Abrir menú"
            className="grid size-8 place-items-center rounded-lg border border-hairline text-ink-muted md:hidden"
          >
            {open ? <X size={15} /> : <Menu size={15} />}
          </button>
        </div>
      </div>

      <nav
        className={cn(
          'flex flex-col gap-1 border-t border-hairline bg-canvas px-5 py-3 md:hidden',
          open ? 'flex' : 'hidden',
        )}
      >
        {items.map((item) => (
          <NavLink
            key={item.to}
            to={item.to}
            onClick={() => setOpen(false)}
            className={({ isActive }) =>
              cn(
                'flex items-center gap-2 rounded-lg px-3 py-2 text-sm font-medium transition-colors',
                isActive
                  ? 'border border-hairline bg-surface-2 text-ink'
                  : 'text-ink-subtle hover:bg-surface-1',
              )
            }
          >
            <item.icon size={15} />
            {item.label}
          </NavLink>
        ))}
      </nav>
    </header>
  )
}
