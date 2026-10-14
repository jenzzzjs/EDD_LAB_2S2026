import { Eye, EyeOff, Lock } from 'lucide-react'
import { useState } from 'react'
import { Input } from '../atoms/Input'

interface PasswordFieldProps {
  id: string
  name?: string
  placeholder?: string
  autoComplete?: string
  required?: boolean
  value?: string
  onChange?: (value: string) => void
}

export function PasswordField({
  id,
  name,
  placeholder = '••••••••',
  autoComplete = 'current-password',
  required,
  value,
  onChange,
}: PasswordFieldProps) {
  const [visible, setVisible] = useState(false)

  return (
    <Input
      id={id}
      name={name}
      type={visible ? 'text' : 'password'}
      placeholder={placeholder}
      autoComplete={autoComplete}
      required={required}
      value={value}
      onChange={onChange ? (e) => onChange(e.target.value) : undefined}
      icon={<Lock size={15} />}
      trailing={
        <button
          type="button"
          onClick={() => setVisible((v) => !v)}
          aria-label={visible ? 'Ocultar contraseña' : 'Mostrar contraseña'}
          className="grid size-7 place-items-center rounded-md text-ink-tertiary transition-colors hover:bg-surface-2 hover:text-ink-muted"
        >
          {visible ? <EyeOff size={15} /> : <Eye size={15} />}
        </button>
      }
    />
  )
}
