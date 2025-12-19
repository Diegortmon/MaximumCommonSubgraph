#include "../include/pheromone.hpp"
#include <algorithm>

namespace mcs {

PheromoneMatrix::PheromoneMatrix(std::size_t n1, std::size_t n2, double init_value)
    : n1_(n1), n2_(n2), pheromone_(n1 * n2, init_value) {}

double PheromoneMatrix::get(Vertex u, Vertex v) const {
    return pheromone_[index(u, v)];
}

void PheromoneMatrix::set(Vertex u, Vertex v, double value) {
    pheromone_[index(u, v)] = value;
}

void PheromoneMatrix::deposit(Vertex u, Vertex v, double amount) {
    pheromone_[index(u, v)] += amount;
}

void PheromoneMatrix::evaporate(double rho) {
    for (auto& tau : pheromone_) {
        tau *= (1.0 - rho);
    }
}

void PheromoneMatrix::reset(double value) {
    std::fill(pheromone_.begin(), pheromone_.end(), value);
}

} // namespace mcs