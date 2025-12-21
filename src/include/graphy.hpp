#pragma once
#include "graph.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace mcs {

class Graphy {
public:
    // Constructor: recibe el grafica Y el mapeo id->nombre
    explicit Graphy(const Graph& graph, 
                   const std::vector<std::string>& id_to_name);

    void dibujaSubgraficaInducidaCircular(
        const std::vector<int>& subset,
        const std::string& output_file
    );

private:
    const Graph& graph_;
    const std::vector<std::string>& id_to_name_;
    std::string svg_;

    static constexpr const char* INICIO =
        "<svg xmlns='http://www.w3.org/2000/svg'";
    static constexpr const char* FINAL = "</svg>\n";

    std::string dimensionaLienzo(int ancho, int alto);
    std::string dibujaArista(
        int x1, int y1, int x2, int y2,
        const std::string& color, int stroke_width
    );
    std::string dibujaVertice(
        const std::string& nombre, int x, int y,
        const std::string& color
    );
    std::string escribe(
        int x, int y, const std::string& texto,
        const std::string& color
    );
};

} // namespace mcs