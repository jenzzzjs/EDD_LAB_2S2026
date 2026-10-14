import sys
import xml.etree.ElementTree as ET

import cairosvg


def buscar_svg(raiz: ET.Element) -> ET.Element:
    for el in raiz.iter():
        if el.tag == "svg":
            return el
    return raiz


def aplicar_estilos(svg: ET.Element) -> None:
    for el in svg.iter():
        clase = el.get("class", "")
        if "aristaLinea" in clase:
            el.set("stroke", "#94a3b8")
            el.set("stroke-width", "2")
        elif "cellSep" in clase:
            el.set("stroke", "#94a3b8")
            el.set("stroke-width", "1")
        elif "nodoCircle" in clase or "nodoRect" in clase:
            el.set("stroke-width", "2")
            if "nodoDesbalanceado" in clase:
                el.set("fill", "#fecaca")
                el.set("stroke", "#b91c1c")
            elif "nodoNormal" in clase:
                el.set("fill", "#bbf7d0")
                el.set("stroke", "#15803d")
            else:
                el.set("fill", "#ffffff")
                el.set("stroke", "#0f172a")
        elif "nodoLabel" in clase or "cellLabel" in clase:
            el.set("font-family", "Arial, sans-serif")
            el.set("font-size", "15")
            el.set("font-weight", "bold")
            el.set("fill", "#0f172a")
            el.set("text-anchor", "middle")
            el.set("dy", "0.325em")


def convertir(svg_path: str, png_path: str) -> None:
    arbol = ET.parse(svg_path)
    svg = buscar_svg(arbol.getroot())
    aplicar_estilos(svg)
    cairosvg.svg2png(bytestring=ET.tostring(svg), write_to=png_path)


if __name__ == "__main__":
    svg = sys.argv[1] if len(sys.argv) > 1 else "si.svg"
    png = sys.argv[2] if len(sys.argv) > 2 else "bpar.png"
    convertir(svg, png)
    print(f"Generado: {png}")