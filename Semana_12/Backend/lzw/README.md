# Archivos de chat LZW

Esta carpeta la crea/usa el backend automáticamente.

- Cada pareja de amigos tiene un archivo `chat_<correoA>__<correoB>_<hash>.lzw`\n  (los correos se ordenan alfabéticamente, así ambos lados usan el mismo archivo).
- El contenido es el historial serializado (una línea JSON por mensaje),
  comprimido con LZW de 12 bits.
- Al abrir un chat, el backend lee el archivo y lo descomprime; al enviar un
  mensaje nuevo, recompone el historial completo y vuelve a escribir el `.lzw`.

No hace falta crear ni editar estos archivos a mano. Se pueden borrar con el
backend detenido si se quiere reiniciar el historial de una conversación.
