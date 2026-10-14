import { Outlet } from 'react-router-dom'
import { MarketingNav } from '../components/organisms/marketing/MarketingNav'

export function MarketingTemplate() {
  return (
    <div className="flex min-h-screen flex-col bg-canvas">
      <MarketingNav />
      <main className="flex-1">
        <Outlet />
      </main>
    </div>
  )
}
