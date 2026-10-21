import { Bell, Inbox, Plus, Search } from 'lucide-react'
import { FriendCard } from '../app/FriendCard'
import { TopologyGraph } from '../app/TopologyGraph'
import { Avatar } from '../../atoms/Avatar'
import { Logo } from '../../atoms/Logo'
import { friends, graphEdges, graphNodes } from '../../../lib/data'

const previewStats = [
  { label: 'Conexiones', value: '42' },
  { label: 'En línea', value: '18' },
  { label: 'Latencia', value: '14 ms' },
]

const navItems = ['Inicio', 'Red', 'Gestión']

/**
 * Mockup de la interfaz real de FaceJens: es el "screenshot de producto"
 * que protagoniza la landing (patrón Linear de producto > ornamento).
 */
export function ProductPreview() {
  return (
    <div className="rounded-2xl border border-hairline bg-surface-1 p-3 shadow-[0_24px_60px_-30px_rgba(0,0,0,0.9)] sm:p-4">
      <div className="overflow-hidden rounded-xl border border-hairline bg-canvas">
        {/* Chrome de la app */}
        <div className="flex items-center justify-between gap-4 border-b border-hairline px-4 py-2.5">
          <div className="flex items-center gap-4">
            <Logo size="sm" to="/app/inicio" />
            <nav className="hidden items-center gap-1 sm:flex">
              {navItems.map((item, index) => (
                <span
                  key={item}
                  className={
                    index === 0
                      ? 'rounded-md bg-surface-2 px-2.5 py-1 text-[11px] font-medium text-ink ring-1 ring-hairline'
                      : 'rounded-md px-2.5 py-1 text-[11px] font-medium text-ink-subtle'
                  }
                >
                  {item}
                </span>
              ))}
            </nav>
          </div>
          <div className="flex items-center gap-2">
            <span className="grid size-6 place-items-center rounded-md border border-hairline text-ink-tertiary">
              <Search size={11} />
            </span>
            <span className="grid size-6 place-items-center rounded-md border border-hairline text-ink-tertiary">
              <Bell size={11} />
            </span>
            <Avatar name="Sofía Morales" size="xs" />
          </div>
        </div>

        {/* Contenido */}
        <div className="grid gap-3 p-3 sm:p-4 lg:grid-cols-[1.35fr_1fr]">
          <div className="flex flex-col gap-3">
            <div className="grid grid-cols-3 gap-2">
              {previewStats.map((stat) => (
                <div
                  key={stat.label}
                  className="rounded-lg border border-hairline bg-surface-1 px-3 py-2"
                >
                  <div className="text-[10px] font-medium text-ink-tertiary">{stat.label}</div>
                  <div className="text-sm font-semibold tracking-tight text-ink">{stat.value}</div>
                </div>
              ))}
            </div>

            <div className="flex items-center justify-between gap-2">
              <span className="text-[11px] font-medium text-ink-subtle">
                Mis conexiones recientes
              </span>
              <span className="inline-flex items-center gap-1 rounded-md bg-primary/10 px-1.5 py-0.5 text-[10px] font-medium text-accent">
                <Plus size={9} /> Agregar
              </span>
            </div>

            <div className="grid gap-2 sm:grid-cols-2">
              {friends.slice(0, 4).map((friend) => (
                <FriendCard key={friend.id} friend={friend} preview />
              ))}
            </div>
          </div>

          <div className="flex flex-col gap-3">
            <div className="rounded-lg border border-hairline bg-surface-1 p-3">
              <div className="mb-2 flex items-center justify-between">
                <span className="text-[11px] font-medium text-ink-subtle">Topología social</span>
                <span className="rounded-full bg-success/10 px-1.5 py-0.5 text-[10px] font-medium text-success">
                  98.4% óptimo
                </span>
              </div>
              <TopologyGraph nodes={graphNodes} edges={graphEdges} compact />
            </div>

            <div className="rounded-lg border border-hairline bg-surface-1 p-3">
              <div className="mb-2 flex items-center justify-between">
                <span className="text-[11px] font-medium text-ink-subtle">Bandeja</span>
                <Inbox size={11} className="text-ink-tertiary" />
              </div>
              <ul className="flex flex-col gap-2">
                {[
                  { who: 'Aoi Tanaka', msg: '¿Revisamos la malla de Tokio?', time: '2 min' },
                  { who: 'Mateo Chen', msg: 'Te envié una invitación al cluster.', time: '18 min' },
                ].map((item) => (
                  <li
                    key={item.who}
                    className="flex items-center gap-2 rounded-md bg-canvas px-2 py-1.5"
                  >
                    <Avatar name={item.who} size="xs" online />
                    <div className="min-w-0 flex-1">
                      <div className="truncate text-[11px] font-medium text-ink">{item.who}</div>
                      <div className="truncate text-[10px] text-ink-tertiary">{item.msg}</div>
                    </div>
                    <span className="text-[9px] text-ink-tertiary">{item.time}</span>
                  </li>
                ))}
              </ul>
            </div>
          </div>
        </div>
      </div>
    </div>
  )
}
