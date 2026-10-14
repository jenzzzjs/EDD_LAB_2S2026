/* Cliente HTTP del backend MVC (Semana 11).
   Todo pasa por aqui: el frontend nunca toca el grafo directamente, solo
   le pide al backend que le devuelva los datos ya armados. */

export const API_BASE =
  (import.meta.env.VITE_API_URL as string | undefined)?.replace(/\/$/, '') ||
  'http://localhost:5000'

export interface BackendUser {
  nombre: string
  correo: string
}

export interface Amigo {
  nombre: string
  correo: string
}

export interface SolicitudPendiente {
  de: string
  nombre: string
  estado: string
}

export interface RedUsuario {
  yo: BackendUser | null
  amigos: Amigo[]
  amigos_en_comun: Array<{ nombre: string; correo: string; conecta_con: string[] }>
  aristas: Array<[string, string]>
  dot: string
  imagen_base64: string | null
  advertencia?: string
}

async function leerError(res: Response): Promise<string> {
  try {
    const data = await res.json()
    if (typeof data?.error === 'string') return data.error
    if (typeof data?.mensaje === 'string') return data.mensaje
  } catch {
    /* cuerpo no JSON */
  }
  return `Error ${res.status}`
}

async function post<T>(ruta: string, cuerpo: unknown): Promise<T> {
  const res = await fetch(`${API_BASE}${ruta}`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(cuerpo),
  })
  const data = (await res.json()) as T & { ok?: boolean; error?: string }
  if (!res.ok || data.ok === false) {
    throw new Error((data as { error?: string }).error || `Error ${res.status}`)
  }
  return data
}

async function get<T>(ruta: string): Promise<T> {
  const res = await fetch(`${API_BASE}${ruta}`)
  const data = (await res.json()) as T & { ok?: boolean; error?: string }
  if (!res.ok || data.ok === false) {
    throw new Error((data as { error?: string }).error || (await leerError(res)))
  }
  return data
}

export const api = {
  // Auth: el registro solo manda nombre, correo y password (nada mas).
  registro(nombre: string, correo: string, password: string) {
    return post<{ ok: boolean; usuario: BackendUser }>('/api/auth/registro', {
      nombre,
      correo,
      password,
    })
  },
  login(correo: string, password: string) {
    return post<{ ok: boolean; usuario: BackendUser }>('/api/auth/login', {
      correo,
      password,
    })
  },
  // Amistades: buscar gente, ver amigos y mover solicitudes
  // (enviar -> pendiente, aceptar/rechazar). La arista nace al aceptar.
  usuarios(excluir = '', q = '') {
    const params = new URLSearchParams()
    if (excluir) params.set('excluir', excluir)
    if (q) params.set('q', q)
    const qs = params.toString()
    return get<{ ok: boolean; usuarios: Array<BackendUser & { es_amigo: boolean }> }>(
      `/api/usuarios${qs ? `?${qs}` : ''}`,
    )
  },
  amigos(correo: string) {
    return get<{ ok: boolean; amigos: Amigo[] }>(
      `/api/amigos?correo=${encodeURIComponent(correo)}`,
    )
  },
  enviarSolicitud(de: string, para: string) {
    return post<{ ok: boolean; mensaje: string }>('/api/solicitudes', { de, para })
  },
  pendientes(correo: string) {
    return get<{ ok: boolean; pendientes: SolicitudPendiente[] }>(
      `/api/solicitudes/pendientes?correo=${encodeURIComponent(correo)}`,
    )
  },
  enviadas(correo: string) {
    return get<{ ok: boolean; enviadas: Array<{ de: string; para: string; estado: string }> }>(
      `/api/solicitudes/enviadas?correo=${encodeURIComponent(correo)}`,
    )
  },
  aceptar(de: string, para: string) {
    return post<{ ok: boolean; mensaje: string }>('/api/solicitudes/aceptar', { de, para })
  },
  rechazar(de: string, para: string) {
    return post<{ ok: boolean; mensaje: string }>('/api/solicitudes/rechazar', { de, para })
  },
  // Grafo: la red ya viene armada del backend (nodos, aristas, DOT y el
  // PNG en base64). Aqui no se construye nada, solo se muestra.
  red(correo: string) {
    return get<RedUsuario>(`/api/grafo/red?correo=${encodeURIComponent(correo)}`)
  },
  /** URL directa del PNG (para <img src> sin pasar por el JSON). */
  redPngUrl(correo: string) {
    return `${API_BASE}/api/grafo/red.png?correo=${encodeURIComponent(correo)}`
  },
}
