import { createBrowserRouter, Navigate } from 'react-router-dom'
import { MarketingTemplate } from './templates/MarketingTemplate'
import { AuthTemplate } from './templates/AuthTemplate'
import { AppTemplate } from './templates/AppTemplate'
import { LandingPage } from './pages/LandingPage'
import { LoginPage } from './pages/LoginPage'
import { RegisterPage } from './pages/RegisterPage'
import { InicioPage } from './pages/app/InicioPage'
import { RedPage } from './pages/app/RedPage'
import { GestionPage } from './pages/app/GestionPage'
import { ChatsPage } from './pages/app/ChatsPage'

export const router = createBrowserRouter([
  {
    element: <MarketingTemplate />,
    children: [{ index: true, element: <LandingPage /> }],
  },
  {
    element: <AuthTemplate />,
    children: [
      { path: '/login', element: <LoginPage /> },
      { path: '/registro', element: <RegisterPage /> },
    ],
  },
  {
    path: '/app',
    element: <AppTemplate />,
    children: [
      { index: true, element: <Navigate to="/app/inicio" replace /> },
      { path: 'inicio', element: <InicioPage /> },
      { path: 'red', element: <RedPage /> },
      { path: 'gestion', element: <GestionPage /> },
      { path: 'chats', element: <ChatsPage /> },
    ],
  },
  { path: '*', element: <Navigate to="/" replace /> },
])
