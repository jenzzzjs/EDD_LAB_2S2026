import { AtSign, LogIn, ShieldCheck } from 'lucide-react'
import { useState } from 'react'
import type { FormEvent } from 'react'
import { Link, useNavigate } from 'react-router-dom'
import { useAuth } from '../lib/auth'
import { Button } from '../components/atoms/Button'
import { Card } from '../components/atoms/Card'
import { FormField } from '../components/molecules/FormField'
import { FormItem, FormStagger } from '../components/molecules/FormStagger'
import { Input } from '../components/atoms/Input'
import { PasswordField } from '../components/molecules/PasswordField'

export function LoginPage() {
  const { loginBackend } = useAuth()
  const navigate = useNavigate()
  const [email, setEmail] = useState('')
  const [password, setPassword] = useState('')
  const [error, setError] = useState<string | null>(null)
  const [loading, setLoading] = useState(false)

  async function handleSubmit(event: FormEvent) {
    event.preventDefault()
    setError(null)
    setLoading(true)
    try {
      await loginBackend(email.trim(), password)
      navigate('/app/inicio')
    } catch (err) {
      setError(err instanceof Error ? err.message : 'No se pudo iniciar sesión')
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
                Acceso seguro v3.4
              </span>
              <h1 className="text-headline text-ink">Bienvenido de nuevo</h1>
              <p className="text-sm leading-relaxed text-ink-subtle">
                Ingresa tus credenciales para acceder a tu malla y la consola de operaciones.
              </p>
            </div>
          </FormItem>

          <FormItem>
            <FormField label="Correo electrónico" hint="Identidad principal" htmlFor="email">
              <Input
                id="email"
                type="email"
                required
                placeholder="tu@correo.com"
                autoComplete="email"
                value={email}
                onChange={(e) => setEmail(e.target.value)}
                icon={<AtSign size={15} />}
              />
            </FormField>
          </FormItem>

          <FormItem>
            <FormField label="Contraseña" hint="Clave de cifrado" htmlFor="password">
              <PasswordField
                id="password"
                required
                autoComplete="current-password"
                value={password}
                onChange={setPassword}
              />
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
              {loading ? 'Verificando…' : 'Iniciar sesión'}
              <LogIn size={16} />
            </Button>
          </FormItem>

          <FormItem>
            <p className="text-center text-[13px] text-ink-subtle">
              ¿No tienes cuenta aún?{' '}
              <Link to="/registro" className="font-medium text-accent hover:underline">
                Regístrate gratis
              </Link>
            </p>
          </FormItem>

          <FormItem>
            <div className="flex items-center justify-center gap-2 rounded-lg border border-hairline bg-canvas px-3 py-2 text-[11px] text-ink-tertiary">
              <ShieldCheck size={13} className="text-secure" />
              Cifrado de extremo a extremo · Protección TLS 1.3
            </div>
          </FormItem>
        </FormStagger>
      </form>
    </Card>
  )
}
