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

export interface ActivityItem {
  id: string
  author: string
  text: string
  accent: 'primary' | 'success' | 'muted'
  time: string
}

export interface Cluster {
  id: string
  name: string
  members: number
  affinity: number
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

export const friends: Friend[] = [
  {
    id: 'f1',
    name: 'Elena Rostova',
    handle: '@elena_ux',
    role: 'Diseñadora UI',
    tags: ['Design Systems'],
    status: 'online',
    mutual: 14,
    recent: true,
    verified: true,
    category: 'Creadores',
    connectedSince: '12 Feb 2026',
  },
  {
    id: 'f2',
    name: 'Carlos Mendoza',
    handle: '@cmendoza_eth',
    role: 'Dev Web3',
    tags: ['Rust & Solidity'],
    status: 'online',
    mutual: 28,
    recent: false,
    category: 'Cohorte',
    connectedSince: '03 Ene 2026',
  },
  {
    id: 'f3',
    name: 'Aoi Tanaka',
    handle: '@aoi_neuro',
    role: 'AI Researcher',
    tags: ['Neural Graphs'],
    status: 'online',
    mutual: 32,
    recent: true,
    category: 'Creadores',
    connectedSince: '27 Mar 2026',
  },
  {
    id: 'f4',
    name: 'David Kim',
    handle: '@dkim_visuals',
    role: 'Fotógrafo',
    tags: ['Visual Art'],
    status: 'offline',
    mutual: 9,
    recent: false,
    category: 'Comunidad',
    connectedSince: '19 Nov 2025',
  },
  {
    id: 'f5',
    name: 'Valentina Díaz',
    handle: '@valen_sound',
    role: 'Sound Designer',
    tags: ['Spatial Audio'],
    status: 'online',
    mutual: 19,
    recent: false,
    category: 'Cohorte',
    connectedSince: '08 Abr 2026',
  },
  {
    id: 'f6',
    name: 'Lucas Silva',
    handle: '@lucas_3d',
    role: '3D Architect',
    tags: ['Unreal Engine 5'],
    status: 'offline',
    mutual: 11,
    recent: true,
    category: 'Comunidad',
    connectedSince: '21 Sep 2025',
  },
]

export const suggestions = [
  { id: 's1', name: 'Mateo Chen', handle: '@m_chen', mutual: 21 },
  { id: 's2', name: "Sara O'Connor", handle: '@sara_art', mutual: 17 },
  { id: 's3', name: 'Julián Brandt', handle: '@brandt_sec', mutual: 8 },
]

export const activity: ActivityItem[] = [
  {
    id: 'a1',
    author: 'Elena Rostova',
    text: 'publicó un nuevo prototipo “Quantum Mesh V3”.',
    accent: 'primary',
    time: 'Hace 8 minutos',
  },
  {
    id: 'a2',
    author: 'Aoi Tanaka',
    text: 'se conectó al cluster de cómputo neural de Tokio.',
    accent: 'muted',
    time: 'Hace 24 minutos',
  },
  {
    id: 'a3',
    author: 'Carlos Mendoza',
    text: 'desplegó 3 contratos en la red de prueba Sepolia.',
    accent: 'success',
    time: 'Hace 1 hora',
  },
]

export const clusters: Cluster[] = [
  { id: 'c1', name: 'GenAI Studio', members: 18, affinity: 92 },
  { id: 'c2', name: 'CyberSecurity Labs', members: 14, affinity: 87 },
  { id: 'c3', name: 'Sound & Synth DAO', members: 16, affinity: 81 },
]

export const graphNodes: GraphNode[] = [
  { id: 'n0', label: 'Tú', x: 50, y: 50, r: 9, status: 'online', group: 'yo' },
  { id: 'n1', label: 'Elena', x: 28, y: 30, r: 6, status: 'online', group: 'amigos' },
  { id: 'n2', label: 'Carlos', x: 72, y: 34, r: 6, status: 'online', group: 'amigos' },
  { id: 'n3', label: 'Aoi', x: 66, y: 68, r: 6, status: 'online', group: 'amigos' },
  { id: 'n4', label: 'Valentina', x: 32, y: 70, r: 6, status: 'online', group: 'amigos' },
  { id: 'n5', label: 'David', x: 14, y: 52, r: 5, status: 'offline', group: 'amigos' },
  { id: 'n6', label: 'Lucas', x: 86, y: 55, r: 5, status: 'offline', group: 'amigos' },
  { id: 'n7', label: 'Mateo', x: 44, y: 16, r: 4, status: 'online', group: 'comunidad' },
  { id: 'n8', label: 'Sara', x: 84, y: 20, r: 4, status: 'online', group: 'comunidad' },
  { id: 'n9', label: 'Julián', x: 18, y: 82, r: 4, status: 'online', group: 'comunidad' },
  { id: 'n10', label: 'Noa', x: 90, y: 78, r: 4, status: 'offline', group: 'comunidad' },
  { id: 'n11', label: 'Iker', x: 56, y: 88, r: 4, status: 'online', group: 'comunidad' },
]

export const graphEdges: Array<[string, string]> = [
  ['n0', 'n1'],
  ['n0', 'n2'],
  ['n0', 'n3'],
  ['n0', 'n4'],
  ['n0', 'n5'],
  ['n0', 'n6'],
  ['n1', 'n7'],
  ['n1', 'n5'],
  ['n2', 'n8'],
  ['n2', 'n6'],
  ['n3', 'n11'],
  ['n3', 'n10'],
  ['n4', 'n9'],
  ['n4', 'n11'],
  ['n5', 'n9'],
  ['n6', 'n10'],
]
