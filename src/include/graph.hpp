#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>

namespace mcs {

using Vertex = int;

class Graph {
public:
    using Vertex = int;

    explicit Graph(std::size_t n);

    void add_edge(Vertex u, Vertex v);

    bool edge(Vertex u, Vertex v) const;

    int degree(Vertex u) const { return degrees_[u]; }  // ← O(1)

    std::size_t num_vertices() const;

private:
    std::size_t n_;
    std::vector<uint8_t> adj_;
    std::vector<int> degrees_;  // ← NUEVO: caché de grados
};

} // namespace mcs
