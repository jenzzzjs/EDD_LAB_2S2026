# Instalacion de Qt (MSYS2 + MinGW)

## 1. Instalar MSYS2

Descargar el instalador desde https://www.msys2.org/ y ejecutarlo.
Se instalara en `C:\msys64` por defecto.

## 2. Instalar Qt6 y herramientas de compilacion

Abrir **MSYS2 UCRT64** (desde el menu de inicio) y ejecutar:

```bash
pacman -S mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-qt6-tools
```

Esto instala las librerias base de Qt6 y las herramientas como Qt Designer y Qt Creator.

## 3. Instalar CMake

En la misma terminal MSYS2 UCRT64:

```bash
pacman -S mingw-w64-ucrt-x86_64-cmake
```

CMake es el sistema de build que usamos para compilar el proyecto.

## 4. Instalar Graphviz (opcional)

Para generar reportes en formato PNG a partir de grafos DOT:

```bash
pacman -S mingw-w64-ucrt-x86_64-graphviz
```

## 5. Configurar el PATH en Windows

Agregar las siguientes rutas al PATH del sistema:

```
C:\msys64\ucrt64\bin
```

Pasos:
1. Buscar **Variables de entorno** en el menu de inicio de Windows
2. En **Variables del sistema**, seleccionar `PATH` y dar clic en **Editar**
3. Dar clic en **Nuevo** y agregar la ruta `C:\msys64\ucrt64\bin`
4. Dar clic en **Aceptar** en todas las ventanas
5. **Cerrar y abrir** cualquier terminal para que los cambios surjan efecto

## 6. Verificar la instalacion

Abrir una terminal (PowerShell o CMD) y ejecutar:

```bash
g++ --version
cmake --version
qmake --version
dot -V
```

Si todos los comandos retornan informacion sin errores, la instalacion esta completa.

## 7. Compilar el proyecto

Desde la carpeta `utilidades/`:

```bash
cd utilidades
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

El ejecutable generado estara en `utilidades/build/PruebaQt.exe`.
Para ejecutar:

```bash
cd utilidades/build
.\PruebaQt.exe
```

## 8. Por que ya no compilamos manualmente como en la Semana 1

En la Semana 1 compilabamos cada archivo `.cpp` directamente con g++:

```bash
g++ archivo.cpp -o archivo.exe
```

Esto funcionaba porque eran archivos sencillos sin dependencias externas.

En la Semana 2 usamos **Qt6**, que tiene muchas librerias (Core, Gui, Widgets, etc.). Compilar manualmente significaria escribir un comando enorme con todas las librerias y rutas:

```bash
g++ vista.cpp sistema.cpp listaenlazada.cpp -o PruebaQt.exe -IC:/msys64/... -LC:/msys64/... -lQt6Core -lQt6Gui -lQt6Widgets
```

**CMake resuelve esto por nosotros.** El archivo `CMakeLists.txt` define:
- Los archivos `.cpp` que componen el proyecto
- Las librerias de Qt que necesitamos
- Las rutas de inclusion

CMake genera automaticamente los comandos de compilacion correctos. Solo ejecutamos:

```bash
cmake --build .
```

Y CMake se encarga de compilar cada archivo `.cpp`, vincular las librerias de Qt y generar el ejecutable `PruebaQt.exe`.
