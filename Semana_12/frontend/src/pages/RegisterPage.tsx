import { ArrowRight, AtSign, Lock, ShieldCheck, User } from 'lucide-react'
import { useState } from 'react'
import type { FormEvent } from 'react'
import { Link, useNavigate } from 'react-router-dom'
import { useAuth } from '../lib/auth'
import { Button } from '../components/atoms/Button'
import { Card } from '../components/atoms/Card'
import { Input } from '../components/atoms/Input'
import { FormField } from '../components/molecules/FormField'
import { FormItem, FormStagger } from '../components/molecules/FormStagger'
import { PasswordField } from '../components/molecules/PasswordField'
import { PasswordStrength } from '../components/molecules/PasswordStrength'

export function RegisterPage() {
  const { registerBackend } = useAuth()
  const navigate = useNavigate()
  // El backend solo maneja estos 3 datos: nombre completo, correo y password.
  const [name, setName] = useState('')
  const [email, setEmail] = useState('')
  const [password, setPassword] = useState('')
  const [error, setError] = useState<string | null>(null)
  const [loading, setLoading] = useState(false)

  async function handleSubmit(event: FormEvent) {
    event.preventDefault()
    setError(null)
    setLoading(true)
    try {
      await registerBackend(name.trim(), email.trim(), password)
      navigate('/app/inicio')
    } catch (err) {
      setError(err instanceof Error ? err.message : 'No se pudo crear la cuenta')
    } finally {
      setLoading(false)
    }
  }

  return (
    <Card className="relative overflow-hidden p-6 sm:p-8">
      <div className="absolute inset-x-0 top-0 h-px bg-linear-to-r from-transparent via-primary/40 to-transparent" />

      <form onSubmit={handleSubmit}>
        <FormStagger className="flex flex-col gap-5">
          <FormItem>
            <div className="flex flex-col gap-2">
              <span className="flex w-fit items-center gap-2 rounded-full border border-hairline bg-surface-2 px-2.5 py-1 text-[11px] font-medium uppercase tracking-[0.08em] text-ink-subtle">
                <span className="size-1.5 animate-pulse rounded-full bg-primary" />
                Acceso temprano v2.4
              </span>
              <h1 className="text-display-md text-ink">Creá tu cuenta</h1>
              <p className="text-sm leading-relaxed text-ink-subtle">
                Únete a la nueva era de conexiones sin fronteras ni ruido.
              </p>
            </div>
          </FormItem>

          <FormItem>
            <FormField label="Nombre completo" hint="Requerido" htmlFor="fullName">
              <Input
                id="fullName"
                name="fullName"
                required
                placeholder="Ej. Sofía Morales"
                autoComplete="name"
                value={name}
                onChange={(e) => setName(e.target.value)}
                icon={<User size={15} />}
              />
            </FormField>
          </FormItem>

          <FormItem>
            <FormField label="Correo electrónico" hint="Institucional o personal" htmlFor="regEmail">
              <Input
                id="regEmail"
                name="email"
                type="email"
                required
                placeholder="sofia@ejemplo.com"
                autoComplete="email"
                value={email}
                onChange={(e) => setEmail(e.target.value)}
                icon={<AtSign size={15} />}
              />
            </FormField>
          </FormItem>

          <FormItem>
            <FormField label="Contraseña" htmlFor="regPassword">
              <PasswordField
                id="regPassword"
                name="password"
                required
                autoComplete="new-password"
                placeholder="••••••••••••"
                value={password}
                onChange={setPassword}
              />
              <PasswordStrength value={password} />
            </FormField>
          </FormItem>

          {error ? (
            <FormItem>
              <p className="rounded-lg border border-danger/30 bg-danger/10 px-3 py-2 text-[13px] text-danger">
                {error}
              </p>
            </FormItem>
          ) : null}

          <FormItem>
            <Button type="submit" size="lg" className="mt-1 w-full" disabled={loading}>
              {loading ? 'Creando cuenta…' : 'Crear cuenta gratis'}
              <ArrowRight size={16} />
            </Button>
          </FormItem>

          <FormItem>
            <div className="flex items-center justify-center gap-2 rounded-lg border border-hairline bg-canvas px-3 py-2 text-[11px] text-ink-tertiary">
              <Lock size={12} className="text-secure" />
              <span className="flex items-center gap-1.5">
                <ShieldCheck size={12} className="text-secure" />
                SSL 256-bit · Privacidad garantizada
              </span>
            </div>
          </FormItem>

          <FormItem>
            <p className="text-center text-[13px] text-ink-subtle">
              ¿Ya tienes una cuenta activa?{' '}
              <Link to="/login" className="font-medium text-accent hover:underline">
                Inicia sesión aquí
              </Link>
            </p>
          </FormItem>
        </FormStagger>
      </form>
    </Card>
  )
}
