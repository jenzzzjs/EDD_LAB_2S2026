import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './index.css'
import { router } from './router'
import { RouterProvider } from 'react-router-dom'
import { MotionConfig } from 'framer-motion'
import { AuthProvider } from './lib/auth'
import { ToastProvider } from './components/molecules/Toast'

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <MotionConfig reducedMotion="user">
      <AuthProvider>
        <ToastProvider>
          <RouterProvider router={router} />
        </ToastProvider>
      </AuthProvider>
    </MotionConfig>
  </StrictMode>,
)
