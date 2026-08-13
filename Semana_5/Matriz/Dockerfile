# La imagen se construye en dos etapas: una compila el programa y la
# otra crea la imagen final sin el compilador.

# en la etapa 1 esta la compilacion
FROM gcc:13-bookworm AS build

# WORKDIR define la carpeta de trabajo donde se ejecutan los comandos.
WORKDIR /app

# Se copia el codigo fuente desde la carpeta del proyecto al contenedor.
COPY Matriz.cpp .

# Se compila con g++ usando enlace estatico para que el binario
# no dependa de librerias externas al ejecutarse.
RUN g++ -static -std=c++17 -O2 Matriz.cpp -o MatrizApp

# en la etapa 2 esta la imagen final con todo lo necesario para ejecutar
FROM alpine:3.22

# Se instala Graphviz para convertir el .dot en .png con la opcion 5.
RUN apk add --no-cache graphviz \
    # Se borran los plugins de Graphviz que el programa no usa para ahorrar espacio.
    && rm -f /usr/lib/graphviz/libgvplugin_neato_layout.so* \
             /usr/lib/graphviz/libgvplugin_kitty.so* \
             /usr/lib/graphviz/libgvplugin_vt.so* \
             /usr/lib/graphviz/libgvplugin_webp.so* \
             /usr/lib/graphviz/libgvplugin_xlib.so* \
             /usr/lib/graphviz/libgvplugin_pango.so* \
    # Se regenera la tabla de plugins para evitar warnings.
    && rm -f /usr/lib/graphviz/config6 \
    && dot -c \
    # Se limpian documentacion y cache de paquetes para ahorrar espacio.
    && rm -rf /usr/share/doc /usr/share/man /var/cache/apk/*

# Se define la carpeta de trabajo de la imagen final.
WORKDIR /app

# Se copian el ejecutable compilado en la etapa 1 y los datos de ejemplo.
COPY --from=build /app/MatrizApp ./MatrizApp
COPY peliculas.csv .

# Se crea la carpeta donde el programa guarda el .dot y el .png.
RUN mkdir -p Matriz

# ENTRYPOINT define el comando que se ejecuta al arrancar el contenedor.
ENTRYPOINT ["./MatrizApp"]
