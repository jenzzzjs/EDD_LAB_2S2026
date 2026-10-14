import { Outlet, Navigate } from 'react-router-dom'
import { useAuth } from '../lib/auth'
import { AppHeader } from '../components/organisms/app/AppHeader'
import { PageTransition } from '../components/molecules/PageTransition'

export function AppTemplate() {
  const { user } = useAuth()

  if (!user) {
    return <Navigate to="/login" replace />
  }

  return (
    <div className="flex min-h-screen flex-col bg-canvas">
      <AppHeader />
      <main className="mx-auto w-full max-w-[1280px] flex-1 px-5 py-8 lg:px-8">
        <PageTransition>
          <Outlet />
        </PageTransition>
      </main>
      <footer className="border-t border-hairline">
        <div className="mx-auto flex max-w-[1280px] flex-col items-center justify-between gap-2 px-5 py-5 text-xs text-ink-tertiary sm:flex-row lg:px-8">
          <span>FaceJens — Ecosistema social hiperconectado</span>
          <span className="flex items-center gap-4">
            <a href="#" className="transition-colors hover:text-ink-muted">
              Privacidad
            </a>
            <a href="#" className="transition-colors hover:text-ink-muted">
              Protocolo
            </a>
            <a href="#" className="transition-colors hover:text-ink-muted">
              Soporte
            </a>
          </span>
        </div>
      </footer>
    </div>
  )
}
