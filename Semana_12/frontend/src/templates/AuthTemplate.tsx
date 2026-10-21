import { Outlet } from 'react-router-dom'
import { MarketingNav } from '../components/organisms/marketing/MarketingNav'
import { PageTransition } from '../components/molecules/PageTransition'

export function AuthTemplate() {
  return (
    <div className="relative flex min-h-screen flex-col bg-canvas">
      {/* Reservas de profundidad: sin gradientes decorativos, solo superficie */}
      <div className="pointer-events-none absolute inset-x-0 top-0 h-px bg-linear-to-r from-transparent via-hairline-strong to-transparent" />

      <MarketingNav />

      <main className="flex flex-1 items-center justify-center px-5 py-10">
        <PageTransition className="w-full max-w-[440px]">
          <Outlet />
        </PageTransition>
      </main>

      <footer className="pb-8 text-center text-xs text-ink-tertiary">
        FaceJens Protocol © 2026 · Cifrado de extremo a extremo
      </footer>
    </div>
  )
}
