import { Search } from 'lucide-react'
import { Input } from '../atoms/Input'

interface SearchBarProps {
  id?: string
  value: string
  onChange: (value: string) => void
  placeholder?: string
  className?: string
}

export function SearchBar({ id, value, onChange, placeholder = 'Buscar…', className }: SearchBarProps) {
  return (
    <Input
      id={id}
      type="search"
      value={value}
      onChange={(e) => onChange(e.target.value)}
      placeholder={placeholder}
      icon={<Search size={15} />}
      className={className}
    />
  )
}
