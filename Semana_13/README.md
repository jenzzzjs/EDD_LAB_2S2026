# Semana 13 — Compresor y descompresor LZW de mensajes

Programa de consola en C++17 que comprime un mensaje con LZW y guarda el
resultado en el archivo `mensaje.lzw`. La descompresión pide la ruta del archivo
que se quiere leer y recupera el mensaje. Usa códigos de 12 bits y guarda el
tamaño original para validar que el resultado sea exactamente el mensaje que se
comprimió. Es independiente de la Semana 12, no necesita servidor ni WebSocket.

## Archivos

- `lzw_mensajes.cpp`: programa completo.
- `README.md`: instrucciones y explicación breve.
- `mensaje.lzw`: archivo que se crea al comprimir un mensaje.

## Compilar y ejecutar

Desde `Semana_13`:

```bash
g++ -std=c++17 -O2 -Wall -Wextra lzw_mensajes.cpp -o lzw_mensajes.exe
```

En Windows PowerShell:

```powershell
.\lzw_mensajes.exe
```

En Linux o macOS:

```bash
./lzw_mensajes
```

## Uso

El programa muestra este menú:

```text
=== LZW de mensajes ===
1. Comprimir un mensaje y guardar mensaje.lzw
2. Descomprimir un archivo indicando su ruta
3. Salir
```

Al elegir la opción `1`, escribe un mensaje. El programa:

- Comprime el mensaje.
- Guarda el resultado en `mensaje.lzw`.
- Muestra el tamaño original y el tamaño comprimido.
- Lee el archivo de nuevo y verifica que el mensaje se recupere correctamente.

Al elegir la opción `2`, el programa pide:

```text
Ruta del archivo a descomprimir:
```

Puedes escribir una ruta relativa o absoluta. Si el archivo está en la misma
carpeta desde donde se ejecuta el programa, basta con escribir `mensaje.lzw`.
También puedes pegar la ruta entre comillas; el programa las quita antes de
abrir el archivo.

El archivo que genera la compresión se guarda en la carpeta desde donde se
ejecuta el programa. Si los comandos anteriores se ejecutan dentro de
`Semana_13`, el archivo queda en `Semana_13/mensaje.lzw`. Si ya existe un
`mensaje.lzw`, se reemplaza.

## Formato del archivo comprimido

```text
"LZW1"                    identificación del formato
uint64 little-endian      tamaño del mensaje original
códigos de 12 bits        empaquetados en bytes consecutivos
```

Los códigos `0` a `255` representan bytes literales. El código `256` es `CLEAR`
y reinicia el diccionario cuando se llena. Los códigos `257` a `4095`
representan secuencias que el algoritmo fue agregando.

## Validaciones

- Rechaza mensajes vacíos.
- Rechaza mensajes mayores a 1 MB.
- Verifica que se pueda crear y escribir `mensaje.lzw`.
- Rechaza una ruta vacía al descomprimir.
- Verifica que se pueda abrir y leer el archivo indicado.
- Rechaza archivos comprimidos mayores a 4 MB.
- Verifica que el archivo empiece con `LZW1`.
- Verifica que el tamaño original sea razonable.
- Rechaza códigos LZW inválidos.
- Comprueba que el texto descomprimido tenga exactamente el tamaño guardado.

## Nota

En mensajes muy cortos el archivo comprimido puede ocupar más que el texto
original. Esto pasa porque el encabezado LZW ocupa 12 bytes y los códigos
necesitan varios bits. El ahorro aparece cuando el mensaje tiene palabras o
secuencias repetidas.
