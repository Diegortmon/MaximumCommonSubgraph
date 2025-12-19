#pragma once
#include "graph.hpp"
#include <vector>

namespace mcs {

class PheromoneMatrix {
public:
    PheromoneMatrix(std::size_t n1, std::size_t n2, double init_value = 1.0);

    double get(Vertex u, Vertex v) const;
    void set(Vertex u, Vertex v, double value);
    void deposit(Vertex u, Vertex v, double amount);
    void evaporate(double rho);
    void reset(double value = 1.0);

    std::size_t size_g1() const { return n1_; }
    std::size_t size_g2() const { return n2_; }

private:
    std::size_t n1_, n2_;
    std::vector<double> pheromone_;

    inline std::size_t index(Vertex u, Vertex v) const {
        return u * n2_ + v;
    }
};

} // namespace mcs