#include "../include/mapping.hpp"
#include "../include/graph.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace mcs {

bool Mapping::is_mapped_g1(Vertex u) const {
    return mapping_.find(u) != mapping_.end();
}

bool Mapping::is_mapped_g2(Vertex v) const {
    return mapped_g2_.find(v) != mapped_g2_.end();
}

Vertex Mapping::get_image(Vertex u) const {
    auto it = mapping_.find(u);
    return (it != mapping_.end()) ? it->second : -1;
}

bool Mapping::is_feasible_add(Vertex u, Vertex v,
                              const Graph& g1,
                              const Graph& g2) const {
    for (const auto& [u2, v2] : mapping_) {
        if (g1.edge(u, u2) != g2.edge(v, v2)) {
            return false;
        }
    }
    return true;
}

void Mapping::add_pair(Vertex u, Vertex v) {
    mapping_[u] = v;
    mapped_g2_.insert(v);
}

void Mapping::remove_pair(Vertex u) {
    auto it = mapping_.find(u);
    if (it == mapping_.end()) return;

    mapped_g2_.erase(it->second);
    mapping_.erase(it);
}

int Mapping::size() const {
    return static_cast<int>(mapping_.size());
}

int Mapping::count_edges(const Graph& g1, const Graph& g2) const {
    int count = 0;
    auto nodes = get_nodes_vector();

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        for (std::size_t j = i + 1; j < nodes.size(); ++j) {
            const auto& [u1, v1] = nodes[i];
            const auto& [u2, v2] = nodes[j];

            if (g1.edge(u1, u2) && g2.edge(v1, v2)) {
                ++count;
            }
        }
    }

    return count;
}

std::vector<std::pair<Vertex, Vertex>> Mapping::get_nodes_vector() const {
    std::vector<std::pair<Vertex, Vertex>> result;
    result.reserve(mapping_.size());
    for (const auto& kv : mapping_) {
        result.emplace_back(kv.first, kv.second);
    }
    return result;
}

void Mapping::clear() {
    mapping_.clear();
    mapped_g2_.clear();
}

// ============================================================================
// NUEVA FUNCIÓN: EXPORTAR MCIS
// ============================================================================
void Mapping::export_mcis(const Graph& g1,
                         const Graph& g2,
                         const std::vector<std::string>& names1,
                         const std::vector<std::string>& names2,
                         const std::string& filename) const {
    if (mapping_.empty()) {
        std::cout << "[Export] Mapeo vacío, no se genera archivo .mcis\n";
        return;
    }

    std::ofstream out(filename);
    if (!out) {
        std::cerr << "[Export] Error: no se pudo crear " << filename << "\n";
        return;
    }

    // Calcular número de aristas
    int num_edges = count_edges(g1, g2);

    // Escribir encabezado
    out << "# Maximum Common Induced Subgraph (MCIS)\n";
    out << "# Vertices: " << mapping_.size() << "\n";
    out << "# Edges: " << num_edges << "\n";
    out << "#\n";
    out << "# Format: Combined vertex names (g1|g2) followed by edge list\n";
    out << "#\n\n";

    // Obtener nodos ordenados
    auto nodes = get_nodes_vector();
    std::sort(nodes.begin(), nodes.end());

    // Crear nombres combinados
    std::vector<std::string> combined_names;
    combined_names.reserve(nodes.size());

    out << "# === VERTEX MAPPING ===\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        const auto& [u, v] = nodes[i];
        
        std::string name_u = (u >= 0 && u < static_cast<int>(names1.size())) 
                            ? names1[u] : std::to_string(u);
        std::string name_v = (v >= 0 && v < static_cast<int>(names2.size())) 
                            ? names2[v] : std::to_string(v);
        
        std::string combined = name_u + "|" + name_v;
        combined_names.push_back(combined);
        
        out << "# " << i << ": " << combined << "\n";
    }
    out << "\n";

    // Escribir aristas
    out << "# === EDGES ===\n";
    int edge_count = 0;
    
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i + 1; j < nodes.size(); ++j) {
            const auto& [u1, v1] = nodes[i];
            const auto& [u2, v2] = nodes[j];

            // Verificar que la arista existe en ambos graficas (MCIS inducido)
            if (g1.edge(u1, u2) && g2.edge(v1, v2)) {
                out << combined_names[i] << " " << combined_names[j] << "\n";
                edge_count++;
            }
        }
    }

    out.close();

    std::cout << "[Export] MCIS guardado en: " << filename << "\n";
    std::cout << "         Vértices: " << mapping_.size() 
              << ", Aristas: " << edge_count << "\n";
}

} // namespace mcs