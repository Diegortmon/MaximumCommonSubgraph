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

    int degree(Vertex u) const;

    std::size_t num_vertices() const;

private:
    std::size_t n_;
    std::vector<uint8_t> adj_;
};

} // namespace mcs
