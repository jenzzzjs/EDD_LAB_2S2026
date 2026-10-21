import { Menu, X } from 'lucide-react'
import { useEffect, useState } from 'react'
import { Link, useLocation, useNavigate } from 'react-router-dom'
import { cn } from '../../../lib/cn'
import { Logo } from '../../atoms/Logo'
import { ButtonLink } from '../../atoms/Button'

const links = [
  { label: 'Producto', href: '#producto' },
  { label: 'Red', href: '#red' },
  { label: 'Comunidad', href: '#comunidad' },
]

export function MarketingNav() {
  const [open, setOpen] = useState(false)
  const [scrolled, setScrolled] = useState(false)
  const navigate = useNavigate()
  const location = useLocation()

  useEffect(() => {
    const onScroll = () => setScrolled(window.scrollY > 24)
    onScroll()
    window.addEventListener('scroll', onScroll, { passive: true })
    return () => window.removeEventListener('scroll', onScroll)
  }, [])

  const goToTop = () => {
    setOpen(false)
    if (location.pathname === '/') {
      window.scrollTo({ top: 0, behavior: 'smooth' })
      return
    }
    navigate('/')
    window.setTimeout(() => window.scrollTo({ top: 0, behavior: 'smooth' }), 200)
  }

  const handleNav = (href: string) => {
    setOpen(false)
    const target = document.querySelector(href)
    if (target) {
      target.scrollIntoView({ behavior: 'smooth', block: 'start' })
      return
    }
    navigate('/')
    window.setTimeout(() => {
      const el = document.querySelector(href)
      if (el) el.scrollIntoView({ behavior: 'smooth', block: 'start' })
    }, 200)
  }

  return (
    <header
      className={cn(
        'sticky top-[1.1rem] z-50 mx-auto mt-[1.1rem] w-[min(1280px,94%)] rounded-[14px] border backdrop-blur-[14px] backdrop-saturate-150 transition-all duration-300',
        scrolled
          ? 'border-[rgba(226,104,92,0.32)] bg-[rgba(12,8,9,0.86)] shadow-[0_18px_44px_-16px_rgba(0,0,0,0.9)]'
          : 'border-[rgba(226,104,92,0.16)] bg-[rgba(12,8,9,0.72)] shadow-[0_12px_32px_-16px_rgba(0,0,0,0.85)]',
        open && 'rounded-b-none',
      )}
    >
      <div className="mx-auto flex h-14 items-center justify-between gap-6 px-4 sm:px-5">
        <Logo size="sm" onClick={goToTop} />

        <nav className="hidden items-center gap-1 md:flex" aria-label="Navegación principal">
          {links.map((link) => (
            <button
              key={link.label}
              type="button"
              onClick={() => handleNav(link.href)}
              className="cursor-pointer rounded-[9px] px-3.5 py-2 text-[13px] font-medium text-ink-subtle transition-colors duration-200 hover:bg-[rgba(226,104,92,0.08)] hover:text-ink"
            >
              {link.label}
            </button>
          ))}
        </nav>

        <div className="flex shrink-0 items-center gap-2">
          <Link
            to="/login"
            className="hidden rounded-[9px] px-3.5 py-2 text-[13px] font-medium text-ink-muted transition-colors duration-200 hover:bg-[rgba(226,104,92,0.08)] hover:text-ink sm:block"
          >
            Iniciar sesión
          </Link>
          <ButtonLink to="/registro" size="sm" className="h-8 rounded-[9px] px-4">
            Registrarse
          </ButtonLink>
        </div>

        <button
          type="button"
          onClick={() => setOpen((v) => !v)}
          className="grid size-9 place-items-center rounded-[9px] border border-hairline text-ink-muted md:hidden"
          aria-label={open ? 'Cerrar menú' : 'Abrir menú'}
        >
          {open ? <X size={16} /> : <Menu size={16} />}
        </button>
      </div>

      {open ? (
        <div className="flex flex-col gap-1 border-t border-hairline px-4 py-3 md:hidden">
          {links.map((link) => (
            <button
              key={link.label}
              type="button"
              onClick={() => {
                setOpen(false)
                handleNav(link.href)
              }}
              className="cursor-pointer rounded-[9px] px-2 py-2 text-left text-sm text-ink-muted transition-colors hover:bg-surface-2 hover:text-ink"
            >
              {link.label}
            </button>
          ))}
          <div className="mt-2 flex items-center gap-2 border-t border-hairline pt-3">
            <Link
              to="/login"
              className="flex-1 rounded-[9px] border border-hairline px-3 py-2 text-center text-[13px] font-medium text-ink"
            >
              Iniciar sesión
            </Link>
            <ButtonLink to="/registro" size="sm" className="h-10 flex-1 rounded-[9px]">
              Registrarse
            </ButtonLink>
          </div>
        </div>
      ) : null}
    </header>
  )
}