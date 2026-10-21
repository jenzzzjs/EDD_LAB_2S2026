import { animate, useInView } from 'framer-motion'
import { useEffect, useRef, useState } from 'react'

interface AnimatedNumberProps {
  value: number
  decimals?: number
  suffix?: string
  className?: string
}

export function AnimatedNumber({
  value,
  decimals = 0,
  suffix = '',
  className,
}: AnimatedNumberProps) {
  const ref = useRef<HTMLSpanElement>(null)
  const inView = useInView(ref, { once: true, margin: '-40px' })
  const [display, setDisplay] = useState(0)

  useEffect(() => {
    if (!inView) return
    const controls = animate(0, value, {
      duration: 1.6,
      ease: [0.16, 1, 0.3, 1],
      onUpdate: (next) => setDisplay(next),
    })
    return () => controls.stop()
  }, [inView, value])

  const formatted =
    decimals > 0
      ? display.toFixed(decimals)
      : Math.round(display).toLocaleString('es-GT')

  return (
    <span ref={ref} className={className}>
      {formatted}
      {suffix}
    </span>
  )
}