#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

namespace mcs {

class Graph;
using Vertex = int;

class Mapping {
public:
    Mapping() = default;

    bool is_mapped_g1(Vertex u) const;
    bool is_mapped_g2(Vertex v) const;

    Vertex get_image(Vertex u) const;

    bool is_feasible_add(Vertex u, Vertex v,
                         const Graph& g1,
                         const Graph& g2) const;

    void add_pair(Vertex u, Vertex v);
    void remove_pair(Vertex u);

    int size() const;

    int count_edges(const Graph& g1, const Graph& g2) const;

    std::vector<std::pair<Vertex, Vertex>> get_nodes_vector() const;

    void clear();

private:
    std::unordered_map<Vertex, Vertex> mapping_;
    std::unordered_set<Vertex> mapped_g2_;
};

} // namespace mcs
