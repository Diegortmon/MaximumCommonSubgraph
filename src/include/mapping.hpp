#pragma once
#include "graph.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>
#include <string>

namespace mcs {

using Vertex = int;

class Mapping {
public:
    bool is_mapped_g1(Vertex u) const;
    bool is_mapped_g2(Vertex v) const;
    Vertex get_image(Vertex u) const;

    bool is_feasible_add(Vertex u, Vertex v,
                        const Graph& g1,
                        const Graph& g2) const;

    void add_pair(Vertex u, Vertex v);
    void remove_pair(Vertex u);
    void clear();

    int size() const;
    int count_edges(const Graph& g1, const Graph& g2) const;

    std::vector<std::pair<Vertex, Vertex>> get_nodes_vector() const;

    // Nueva función: exportar el MCIS como archivo de aristas
    void export_mcis(const Graph& g1,
                    const Graph& g2,
                    const std::vector<std::string>& names1,
                    const std::vector<std::string>& names2,
                    const std::string& filename) const;

private:
    std::unordered_map<Vertex, Vertex> mapping_;
    std::unordered_set<Vertex> mapped_g2_;
};

} // namespace mcs