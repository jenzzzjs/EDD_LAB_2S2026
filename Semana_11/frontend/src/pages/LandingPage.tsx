import { ArrowRight, Fingerprint, Network, ShieldCheck, Users } from 'lucide-react'
import { motion } from 'framer-motion'
import type { Variants } from 'framer-motion'
import { ButtonLink } from '../components/atoms/Button'
import { Badge } from '../components/atoms/Badge'
import { Card } from '../components/atoms/Card'
import { AnimatedNumber } from '../components/molecules/AnimatedNumber'
import { PageHeader } from '../components/molecules/PageHeader'
import { PageTransition } from '../components/molecules/PageTransition'
import { Reveal } from '../components/molecules/Reveal'
import { StatTile } from '../components/molecules/StatTile'
import { HeroVisual } from '../components/organisms/marketing/HeroVisual'
import { ProductPreview } from '../components/organisms/marketing/ProductPreview'
import { TickerBand } from '../components/organisms/marketing/TickerBand'
import { TopologyGraph } from '../components/organisms/app/TopologyGraph'
import { cn } from '../lib/cn'
import { clusters, graphEdges, graphNodes } from '../lib/data'

const heroStats: Array<{
  value: number
  decimals?: number
  suffix?: string
  label: string
}> = [
  { value: 148, label: 'nodos activos' },
  { value: 32, label: 'canales P2P' },
  { value: 14, suffix: ' ms', label: 'latencia media' },
  { value: 99.98, decimals: 2, suffix: '%', label: 'uptime' },
]

const features = [
  {
    icon: Network,
    title: 'Malla P2P',
    body: 'Tu grafo de contactos se propaga sin centros ni intermediarios. Ves quién está conectado, con qué fuerza y en qué cluster vive.',
    meta: 'protocolo v4.8 · sync 240 ms',
  },
  {
    icon: Users,
    title: 'Gestión sin fricción',
    body: 'Agrega, silencia o desconecta nodos en segundos. Solicitudes, categorías y capacidad de red en un solo panel.',
    meta: 'capacidad 250 nodos',
  },
  {
    icon: ShieldCheck,
    title: 'Privacidad por diseño',
    body: 'Cifrado de extremo a extremo y sesiones zero-knowledge. Tus conexiones no son el producto: son tuyas.',
    meta: 'tls 1.3 · e2e',
  },
]

const headlineLeft = ['La', 'red', 'social', 'que', 'se', 'dibuja', 'como']
const headlineRight = ['una', 'malla', 'viva.']

const staggerVariants: Variants = {
  hidden: {},
  show: { transition: { staggerChildren: 0.09, delayChildren: 0.05 } },
}

const wordVariants: Variants = {
  hidden: { opacity: 0, y: 18, filter: 'blur(6px)' },
  show: {
    opacity: 1,
    y: 0,
    filter: 'blur(0px)',
    transition: { duration: 0.55, ease: [0.16, 1, 0.3, 1] },
  },
}

function HeroWord({ word, accent = false }: { word: string; accent?: boolean }) {
  return (
    <motion.span
      variants={wordVariants}
      className={cn('mr-[0.24em] inline-block', accent && 'text-gradient-brand')}
    >
      {word}
    </motion.span>
  )
}

export function LandingPage() {
  return (
    <PageTransition>
      {/* ============ HERO: mensaje + degradado con imagen ============ */}
      <section className="relative isolate overflow-hidden">
        <HeroVisual />

        <div className="relative mx-auto grid min-h-[90vh] max-w-[1280px] items-center gap-10 px-5 py-24 lg:grid-cols-2 lg:px-8 lg:py-28">
          <div className="flex max-w-xl flex-col gap-7">
            <motion.div
              initial={{ opacity: 0, y: 12 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, ease: [0.16, 1, 0.3, 1] }}
            >
              <Badge tone="accent" className="border-accent/30 bg-accent/10">
                <span className="size-1.5 animate-pulse rounded-full bg-primary-focus" />
                FaceJens · beta abierta
              </Badge>
            </motion.div>

            <motion.h1
              variants={staggerVariants}
              initial="hidden"
              animate="show"
              className="text-display-xl text-ink"
            >
              <span className="block">
                {headlineLeft.map((word) => (
                  <HeroWord key={word} word={word} />
                ))}
              </span>
              <span className="block">
                {headlineRight.map((word) => (
                  <HeroWord key={word} word={word} accent />
                ))}
              </span>
            </motion.h1>

            <motion.p
              initial={{ opacity: 0, y: 14 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.55, delay: 0.85, ease: [0.16, 1, 0.3, 1] }}
              className="max-w-lg text-base leading-relaxed text-ink-subtle sm:text-lg"
            >
              FaceJens organiza tus conexiones como un grafo en vivo: quién está en línea, con
              qué fuerza y en qué cluster vive. Sin algoritmos opacos ni ruido — solo tu red, en
              rojo y negro.
            </motion.p>

            <motion.div
              initial={{ opacity: 0, y: 14 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.55, delay: 1, ease: [0.16, 1, 0.3, 1] }}
              className="flex flex-wrap items-center gap-3"
            >
              <ButtonLink to="/registro" size="lg">
                Crear cuenta gratis
                <ArrowRight size={16} />
              </ButtonLink>
              <ButtonLink to="/login" size="lg" variant="ghost">
                Ya tengo cuenta
              </ButtonLink>
            </motion.div>

            <motion.dl
              initial={{ opacity: 0, y: 14 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.55, delay: 1.15, ease: [0.16, 1, 0.3, 1] }}
              className="flex flex-wrap gap-x-8 gap-y-4 border-t border-hairline pt-6"
            >
              {heroStats.map((stat) => (
                <div key={stat.label} className="flex flex-col gap-1">
                  <dt className="order-2 text-[11px] font-medium uppercase tracking-[0.12em] text-ink-tertiary">
                    {stat.label}
                  </dt>
                  <dd className="order-1">
                    <AnimatedNumber
                      value={stat.value}
                      decimals={stat.decimals ?? 0}
                      suffix={stat.suffix ?? ''}
                      className="text-2xl font-bold tracking-[-0.03em] text-ink"
                    />
                  </dd>
                </div>
              ))}
            </motion.dl>
          </div>

          {/* Espacio reservado para el arte de la derecha */}
          <div className="hidden lg:block" />
        </div>
      </section>

      <TickerBand />

      {/* ============ EL PRODUCTO ============ */}
      <section id="producto" className="mx-auto max-w-[1280px] scroll-mt-24 px-5 py-24 lg:px-8">
        <Reveal>
          <PageHeader
            eyebrow="El producto"
            title="Todo tu grafo, en una sola pantalla"
            description="Inicio, Red y Gestión: tres vistas al mismo grafo vivo. Sin feeds infinitos que te atrapen — solo el estado real de tus conexiones."
          />
        </Reveal>

        <Reveal delay={0.1} className="mt-12">
          <ProductPreview />
        </Reveal>
      </section>

      {/* ============ FEATURES ============ */}
      <section id="red" className="mx-auto max-w-[1280px] scroll-mt-24 px-5 pb-24 lg:px-8">
        <Reveal>
          <PageHeader
            eyebrow="Diseñada como una malla"
            title="Lo que una red necesita, nada que la ensucie"
            description="Tres pilares sostienen FaceJens: una topología viva, herramientas de gestión directas y privacidad que no se negocia."
          />
        </Reveal>

        <div className="mt-10 grid gap-5 md:grid-cols-3">
          {features.map((feature, index) => (
            <Reveal key={feature.title} delay={index * 0.08}>
              <Card
                hover
                className="group flex h-full flex-col gap-4 p-6 transition-all duration-300 hover:border-primary/40 hover:shadow-[0_0_50px_-16px_rgba(179,45,35,0.5)]"
              >
                <span className="grid size-10 place-items-center rounded-lg border border-primary/25 bg-primary/10 text-accent transition-transform duration-300 group-hover:scale-105">
                  <feature.icon size={18} />
                </span>
                <div className="flex flex-col gap-2">
                  <h3 className="text-card-title text-ink">{feature.title}</h3>
                  <p className="text-sm leading-relaxed text-ink-subtle">{feature.body}</p>
                </div>
                <span className="mt-auto border-t border-hairline pt-3 font-mono text-xs text-ink-tertiary">
                  {feature.meta}
                </span>
              </Card>
            </Reveal>
          ))}
        </div>
      </section>

      {/* ============ GRAFO EN VIVO ============ */}
      <section id="comunidad" className="scroll-mt-24 border-y border-hairline bg-surface-1/40">
        <div className="mx-auto grid max-w-[1280px] items-center gap-10 px-5 py-24 lg:grid-cols-2 lg:px-8">
          <Reveal>
            <div className="flex flex-col gap-5">
              <Badge tone="accent" className="w-max">
                <span className="size-1.5 animate-pulse rounded-full bg-primary-focus" />
                Topología en vivo
              </Badge>
              <h2 className="text-display-lg text-ink">Ve tu red como realmente es</h2>
              <p className="text-base leading-relaxed text-ink-subtle">
                Cada nodo tiene peso, latencia y afinidad. La vista de malla te muestra clusters,
                puentes y vecinos débiles antes de que se conviertan en conexiones perdidas.
              </p>
              <ul className="flex flex-col gap-3 pt-1">
                {[
                  'Nodos con estado en línea y último ping',
                  'Filtros por afinidad, cluster y co-creadores',
                  'Modo resonancia para seguir el trazo de señal',
                ].map((item) => (
                  <li key={item} className="flex items-start gap-2.5 text-sm text-ink-muted">
                    <span className="mt-1.5 size-1.5 shrink-0 rounded-full bg-primary" />
                    {item}
                  </li>
                ))}
              </ul>
              <div className="pt-2">
                <ButtonLink to="/registro" variant="secondary">
                  Explorar la malla
                  <ArrowRight size={15} />
                </ButtonLink>
              </div>
            </div>
          </Reveal>

          <Reveal delay={0.1}>
            <Card className="relative overflow-hidden p-5">
              <div
                aria-hidden="true"
                className="pointer-events-none absolute inset-0 opacity-60"
                style={{
                  background:
                    'radial-gradient(60% 50% at 60% 20%, rgba(179,45,35,0.28) 0%, transparent 70%)',
                  maskImage: 'linear-gradient(180deg, #000 0%, transparent 100%)',
                }}
              />
              <div className="relative mb-4 flex items-center justify-between">
                <span className="text-xs font-medium text-ink-subtle">Muestra de malla</span>
                <Badge tone="success">
                  <span className="size-1.5 rounded-full bg-success" />
                  98.4% óptimo
                </Badge>
              </div>
              <div className="relative h-[320px] rounded-lg border border-hairline bg-canvas p-3">
                <TopologyGraph nodes={graphNodes} edges={graphEdges} resonance />
              </div>
              <div className="relative mt-4 grid grid-cols-3 gap-2">
                <StatTile label="Nodos" value="48" detail="+6 hoy" />
                <StatTile label="Densidad" value="78%" />
                <StatTile label="Puentes" value="19" />
              </div>
            </Card>
          </Reveal>
        </div>
      </section>

      {/* ============ CLUSTERS ============ */}
      <section className="mx-auto max-w-[1280px] px-5 py-24 lg:px-8">
        <Reveal>
          <PageHeader
            eyebrow="Comunidades activas"
            title="Clusters donde ya está tu gente"
            description="Únete a subredes con afinidad alta y descubrí nodos que comparten tu misma disciplina."
          />
        </Reveal>
        <div className="mt-8 grid gap-5 md:grid-cols-3">
          {clusters.map((cluster, index) => (
              <Reveal key={cluster.id} delay={index * 0.07}>
                <Card hover className="flex flex-col gap-3 p-6">
                  <div className="flex items-center justify-between">
                    <h3 className="text-[15px] font-semibold text-ink">{cluster.name}</h3>
                    <Badge tone="accent">{cluster.affinity}% afinidad</Badge>
                  </div>
                  <span className="text-sm text-ink-subtle">
                    {cluster.members} nodos activos en la subred
                  </span>
                  <div className="mt-1 h-1 overflow-hidden rounded-full bg-surface-3">
                    <div
                      className="relative h-full rounded-full bg-primary"
                      style={{ width: `${cluster.affinity}%` }}
                    >
                      <span className="absolute inset-0 animate-pulse rounded-full bg-accent/40" />
                    </div>
                  </div>
                </Card>
              </Reveal>
            ))}
        </div>
      </section>

      {/* ============ CTA FINAL ============ */}
      <section className="mx-auto max-w-[1280px] px-5 pb-24 lg:px-8">
        <Reveal>
          <div className="relative overflow-hidden rounded-2xl border border-hairline px-6 py-16 text-center">
            <div
              aria-hidden="true"
              className="pointer-events-none absolute inset-0"
              style={{
                background:
                  'radial-gradient(70% 120% at 50% 0%, rgba(179,45,35,0.35) 0%, rgba(179,45,35,0.06) 42%, transparent 70%), linear-gradient(180deg, #1c0c0a 0%, #0a0708 100%)',
              }}
            />
            <div
              aria-hidden="true"
              className="pointer-events-none absolute inset-x-12 top-0 h-px"
              style={{ background: 'linear-gradient(90deg, transparent, rgba(226,104,92,0.6), transparent)' }}
            />
            <div className="relative flex flex-col items-center gap-5">
              <Fingerprint size={22} className="text-accent" />
              <h2 className="max-w-2xl text-display-md text-ink">
                Empezá a tejer tu <span className="text-gradient-brand">malla</span> hoy mismo
              </h2>
              <p className="max-w-md text-sm leading-relaxed text-ink-subtle">
                Creá tu cuenta, importá tus conexiones y mirá cómo tu red toma forma en menos de
                dos minutos.
              </p>
              <div className="flex flex-wrap items-center justify-center gap-3">
                <ButtonLink to="/registro" size="lg">
                  Crear cuenta gratis
                  <ArrowRight size={16} />
                </ButtonLink>
                <ButtonLink to="/login" variant="ghost" size="lg">
                  Ya tengo cuenta
                </ButtonLink>
              </div>
            </div>
          </div>
        </Reveal>
      </section>
    </PageTransition>
  )
}