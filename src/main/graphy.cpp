#include "../include/graphy.hpp"
#include "../include/graph.hpp"
#include <fstream>
#include <cmath>
#include <iostream>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace mcs {

Graphy::Graphy(const Graph& graph, 
               const std::vector<std::string>& id_to_name) 
    : graph_(graph), id_to_name_(id_to_name) {}

std::string Graphy::dimensionaLienzo(int ancho, int alto) {
    return " width='" + std::to_string(ancho) +
           "' height='" + std::to_string(alto) + "'>\n";
}

std::string Graphy::dibujaArista(
    int x1, int y1, int x2, int y2,
    const std::string& color, int stroke_width
) {
    return "\t<line x1='" + std::to_string(x1) +
           "' y1='" + std::to_string(y1) +
           "' x2='" + std::to_string(x2) +
           "' y2='" + std::to_string(y2) +
           "' stroke='" + color +
           "' stroke-width='" + std::to_string(stroke_width) + "' />\n";
}

std::string Graphy::escribe(
    int x, int y, const std::string& texto,
    const std::string& color
) {
    return "\t<text x='" + std::to_string(x) +
           "' y='" + std::to_string(y) +
           "' fill='" + color +
           "' font-family='Arial' font-size='14' text-anchor='middle'>" +
           texto + "</text>\n";
}

std::string Graphy::dibujaVertice(
    const std::string& nombre, int x, int y,
    const std::string& color
) {
    std::string s =
        "\t<circle cx='" + std::to_string(x) +
        "' cy='" + std::to_string(y) +
        "' r='20' fill='white' stroke='black' stroke-width='2'/>\n";
    s += escribe(x, y + 5, nombre, color);
    return s;
}

void Graphy::dibujaSubgrafoInducidoCircular(
    const std::vector<int>& subset,
    const std::string& output_file
) {
    const int k = static_cast<int>(subset.size());
    if (k == 0) {
        std::cout << "[Graphy] Subgrafo vacío, no se genera SVG.\n";
        return;
    }

    const int radio = std::max(200, k * 15);
    const int ancho = 2 * radio + 100;
    const int alto  = ancho;
    const int cx = ancho / 2;
    const int cy = alto / 2;

    std::vector<int> xs(k), ys(k);
    const double delta = 2.0 * M_PI / k;

    for (int i = 0; i < k; ++i) {
        double angulo = i * delta - M_PI / 2.0;  // Empezar arriba
        xs[i] = static_cast<int>(cx + radio * std::cos(angulo));
        ys[i] = static_cast<int>(cy + radio * std::sin(angulo));
    }

    svg_.clear();
    svg_ += INICIO;
    svg_ += dimensionaLienzo(ancho, alto);

    // Dibujar aristas inducidas
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            int u = subset[i];
            int v = subset[j];

            if (graph_.edge(u, v)) {
                svg_ += dibujaArista(
                    xs[i], ys[i],
                    xs[j], ys[j],
                    "#2563eb", 2
                );
            }
        }
    }

    // Dibujar vértices con nombres
    for (int i = 0; i < k; ++i) {
        int vertex_id = subset[i];
        
        // Validar que el id existe en el mapeo
        std::string nombre;
        if (vertex_id >= 0 && 
            vertex_id < static_cast<int>(id_to_name_.size())) {
            nombre = id_to_name_[vertex_id];
        } else {
            nombre = std::to_string(vertex_id);  // Fallback
        }
        
        svg_ += dibujaVertice(nombre, xs[i], ys[i], "black");
    }

    svg_ += FINAL;

    std::ofstream out(output_file);
    if (!out) {
        std::cerr << "[Graphy] Error: no se pudo crear " 
                  << output_file << "\n";
        return;
    }
    
    out << svg_;
    out.close();

    std::cout << "[Graphy] Subgrafo MCIS guardado en: "
              << output_file << " (" << k << " vértices)\n";
}

} // namespace mcs