#include "../include/graph.hpp"

namespace mcs {

Graph::Graph(std::size_t n) : n_(n), adj_(n * n, 0), degrees_(n, 0) {}

void Graph::add_edge(Vertex u, Vertex v) {
    adj_[u * n_ + v] = 1;
    adj_[v * n_ + u] = 1;
    degrees_[u]++;
    degrees_[v]++;
}

bool Graph::edge(Vertex u, Vertex v) const {
    return adj_[u * n_ + v];
}


std::size_t Graph::num_vertices() const {
    return n_;
}

} // namespace mcs
