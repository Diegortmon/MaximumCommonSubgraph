#include "../include/mapping.hpp"
#include "../include/graph.hpp"

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
    // MCIS inducido incremental:
    // para todo (u2 -> v2) ya en el mapeo,
    // debe cumplirse:
    // (u,u2) ∈ E1  <=>  (v,v2) ∈ E2
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

} // namespace mcs
