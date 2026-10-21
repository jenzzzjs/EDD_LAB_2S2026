#pragma once
// LZW es un algoritmo de compresion sin perdida. La idea principal es
// construir un diccionario de secuencias de bytes mientras se recorre el
// texto. Al inicio el diccionario tiene los 256 valores posibles de un byte.
// Cada vez que aparece una secuencia nueva se le asigna un codigo. Cuando la
// misma secuencia vuelve a aparecer, en lugar de escribir todos sus bytes se
// escribe su codigo, que ocupa 12 bits.
//
// El archivo comprimido tiene este formato:
//
//   "LZW1"                       cuatro bytes de identificacion
//   uint64 little-endian         cantidad de bytes del texto original
//   codigos de 12 bits           empaquetados en bytes consecutivos
//
// Los codigos 0 a 255 representan los bytes literales. El codigo 256 es
// CLEAR y sirve para reiniciar el diccionario cuando se llena. Los codigos
// 257 a 4095 representan secuencias que el algoritmo fue agregando.
//
// Para comprimir recorremos el texto y mantenemos la secuencia mas larga que
// ya existe en el diccionario. Cuando el siguiente byte forma una secuencia
// desconocida, escribimos el codigo de la secuencia actual, agregamos la
// secuencia nueva al diccionario y empezamos de nuevo con ese byte. Para
// descomprimir hacemos el proceso inverso: leemos codigos, reconstruimos las
// secuencias y armamos el diccionario en el mismo orden en que lo armo el
// compresor.

#include <string>

namespace lzw {

// Comprime el texto y deja el resultado en salida. Devuelve false si el texto
// supera el limite permitido o si ocurre un problema interno.
bool comprimir(const std::string& texto, std::string& salida, std::string& error);

// Descomprime el contenido de un archivo LZW y deja el texto original en
// texto. Devuelve false si el archivo no tiene el formato esperado, si esta
// truncado o si el resultado no coincide con el tamano guardado.
bool descomprimir(const std::string& entrada, std::string& texto, std::string& error);

}  // namespace lzw
