#pragma once
#include "graph.hpp"
#include "mapping.hpp"
#include "pheromone.hpp"
#include <random>
#include <vector>
#include <utility>

namespace mcs {

class Ant {
public:
    explicit Ant(std::mt19937& rng) : rng_(rng) {}

    Mapping construct_solution(
        const Graph& g1,
        const Graph& g2,
        const PheromoneMatrix& pheromone,
        double alpha,
        double beta
    );

private:
    std::mt19937& rng_;

    // Calcula heurística combinada: verificación + grado
    double compute_heuristic(
        Vertex u,
        Vertex v,
        const Mapping& current,
        const Graph& g1,
        const Graph& g2
    ) const;

    std::pair<Vertex, Vertex> select_next_pair(
        const Mapping& current,
        const Graph& g1,
        const Graph& g2,
        const PheromoneMatrix& pheromone,
        double alpha,
        double beta
    );

    std::vector<double> compute_probabilities(
        const std::vector<std::pair<Vertex, Vertex>>& candidates,
        const Mapping& current,
        const Graph& g1,
        const Graph& g2,
        const PheromoneMatrix& pheromone,
        double alpha,
        double beta
    ) const;
};

} // namespace mcs