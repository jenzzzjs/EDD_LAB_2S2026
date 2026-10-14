import { cn } from '../../lib/cn'

export function passwordScore(value: string): number {
  if (!value) return 0
  let score = 0
  if (value.length >= 6) score++
  if (value.length >= 10 && /[A-Z]/.test(value) && /[0-9]/.test(value)) score++
  if (value.length >= 12 && /[^A-Za-z0-9]/.test(value)) score++
  return score
}

const labels = ['Introduce una clave segura', 'Débil', 'Media', 'Fuerte']
const tones = ['text-ink-tertiary', 'text-danger', 'text-warning', 'text-success']
const bars = ['bg-hairline', 'bg-danger', 'bg-warning', 'bg-success']

export function PasswordStrength({ value }: { value: string }) {
  const score = passwordScore(value)
  return (
    <div className="flex flex-col gap-1.5">
      <div className="grid grid-cols-3 gap-1.5">
        {[1, 2, 3].map((step) => (
          <span
            key={step}
            className={cn('h-1 rounded-full transition-colors duration-300', bars[score >= step ? score : 0])}
          />
        ))}
      </div>
      <div className="flex items-center justify-between">
        <span className={cn('text-xs font-medium', tones[score])}>{labels[score]}</span>
        <span className="text-xs text-ink-tertiary">Mínimo 8 caracteres</span>
      </div>
    </div>
  )
}
