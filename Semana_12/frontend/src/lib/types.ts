/* Tipos compartidos (sin datos). Los mocks vivian en lib/data.ts. */

export type ConnectionStatus = 'online' | 'offline'

export interface Friend {
  id: string
  name: string
  handle: string
  role: string
  tags: string[]
  status: ConnectionStatus
  mutual: number
  recent: boolean
  verified?: boolean
  category: string
  connectedSince: string
}

export interface GraphNode {
  id: string
  label: string
  x: number
  y: number
  r: number
  status: ConnectionStatus
  group: 'yo' | 'amigos' | 'comunidad'
}
