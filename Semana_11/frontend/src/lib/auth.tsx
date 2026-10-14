import { createContext, useCallback, useContext, useEffect, useMemo, useState } from 'react'
import type { ReactNode } from 'react'
import { api } from './api'

export interface SessionUser {
  name: string
  email: string
  handle: string
}

interface AuthContextValue {
  user: SessionUser | null
  /** Sesion local sin backend (quedo por compatibilidad, ya no se usa). */
  login: (email: string, name?: string) => void
  /** Login de verdad: POST /api/auth/login {correo, password}. */
  loginBackend: (correo: string, password: string) => Promise<void>
  /** Registro de verdad: POST /api/auth/registro {nombre, correo, password}. */
  registerBackend: (nombre: string, correo: string, password: string) => Promise<void>
  logout: () => void
}

const STORAGE_KEY = 'FaceJens.session'

const AuthContext = createContext<AuthContextValue | null>(null)

function readStoredSession(): SessionUser | null {
  try {
    const raw = localStorage.getItem(STORAGE_KEY)
    if (!raw) return null
    return JSON.parse(raw) as SessionUser
  } catch {
    return null
  }
}

function buildUser(email: string, name?: string): SessionUser {
  const clean = email.trim()
  const baseName = name?.trim() || clean.split('@')[0] || 'jens'
  const displayName = baseName
    .split(/[._-]+/)
    .filter(Boolean)
    .map((part) => part.charAt(0).toUpperCase() + part.slice(1))
    .join(' ')
  const handle = '@' + (clean.split('@')[0] || 'jens').replace(/[^a-z0-9_]/gi, '').toLowerCase()
  return { name: displayName || 'jens', email: clean, handle }
}

export function AuthProvider({ children }: { children: ReactNode }) {
  const [user, setUser] = useState<SessionUser | null>(() => readStoredSession())

  useEffect(() => {
    if (user) {
      localStorage.setItem(STORAGE_KEY, JSON.stringify(user))
    } else {
      localStorage.removeItem(STORAGE_KEY)
    }
  }, [user])

  const login = useCallback((email: string, name?: string) => {
    setUser(buildUser(email, name))
  }, [])

  const loginBackend = useCallback(async (correo: string, password: string) => {
    const data = await api.login(correo, password)
    setUser(buildUser(data.usuario.correo, data.usuario.nombre))
  }, [])

  const registerBackend = useCallback(async (nombre: string, correo: string, password: string) => {
    const data = await api.registro(nombre, correo, password)
    setUser(buildUser(data.usuario.correo, data.usuario.nombre))
  }, [])

  const logout = useCallback(() => setUser(null), [])

  const value = useMemo(
    () => ({ user, login, loginBackend, registerBackend, logout }),
    [user, login, loginBackend, registerBackend, logout],
  )

  return <AuthContext.Provider value={value}>{children}</AuthContext.Provider>
}

export function useAuth(): AuthContextValue {
  const ctx = useContext(AuthContext)
  if (!ctx) throw new Error('useAuth debe usarse dentro de <AuthProvider>')
  return ctx
}
