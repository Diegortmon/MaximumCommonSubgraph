#include "../include/ant.hpp"
#include "../include/graph.hpp"

#include <algorithm>
#include <cmath>
#include <random>

namespace mcs {

Mapping Ant::construct_solution(
    const Graph& g1,
    const Graph& g2,
    const PheromoneMatrix& pheromone,
    double alpha,
    double beta
) {
    Mapping mapping;

    const int max_steps =
        static_cast<int>(std::min(g1.num_vertices(), g2.num_vertices()));

    for (int step = 0; step < max_steps; ++step) {
        auto [u, v] = select_next_pair(
            mapping, g1, g2, pheromone, alpha, beta
        );

        if (u == -1) break;

        // Factibilidad estricta (MCIS inducido)
        if (!mapping.is_feasible_add(u, v, g1, g2))
            continue;

        mapping.add_pair(u, v);
    }

    return mapping;
}

double Ant::compute_heuristic(
    Vertex u,
    Vertex v,
    const Mapping& current,
    const Graph& g1,
    const Graph& g2
) const {
    // Primer par: coincidencia estructural gruesa
    if (current.size() == 0) {
        int deg_u = g1.degree(u);
        int deg_v = g2.degree(v);

        int diff = std::abs(deg_u - deg_v);
        double similarity = 1.0 / (1.0 + diff);

        return (deg_u + deg_v) * similarity + 1.0;
    }

    int preserved = 0;
    int checked   = 0;

    for (const auto& [um, vm] : current.get_nodes_vector()) {
        bool e1 = g1.edge(u, um);
        bool e2 = g2.edge(v, vm);

        ++checked;

        // SOLO cuenta aristas preservadas
        if (e1 && e2)
            ++preserved;
    }

    double density =
        static_cast<double>(preserved) / std::max(1, checked);

    double degree_bonus =
        0.1 * (g1.degree(u) + g2.degree(v));

    // Heurística estructural fuerte, pero suave
    return (preserved + 1.0) * (1.0 + density) + degree_bonus;
}

std::pair<Vertex, Vertex> Ant::select_next_pair(
    const Mapping& current,
    const Graph& g1,
    const Graph& g2,
    const PheromoneMatrix& pheromone,
    double alpha,
    double beta
) {
    std::vector<std::pair<Vertex, Vertex>> candidates;

    for (Vertex u = 0; u < static_cast<Vertex>(g1.num_vertices()); ++u) {
        if (current.is_mapped_g1(u)) continue;

        for (Vertex v = 0; v < static_cast<Vertex>(g2.num_vertices()); ++v) {
            if (current.is_mapped_g2(v)) continue;

            if (current.is_feasible_add(u, v, g1, g2))
                candidates.emplace_back(u, v);
        }
    }

    if (candidates.empty())
        return {-1, -1};

    // Refuerzo temprano (B ligero)
    double beta_local = beta;
    if (current.size() < 5)
        beta_local = beta * 1.5;

    // Si hay pocos candidatos, elegir el mejor directamente
    if (candidates.size() <= 3) {
        double best = -1.0;
        std::pair<Vertex, Vertex> best_pair{-1, -1};

        for (const auto& [u, v] : candidates) {
            double tau = pheromone.get(u, v);
            double eta = compute_heuristic(u, v, current, g1, g2);

            double value =
                std::pow(std::max(tau, 1e-6), alpha) *
                std::pow(eta, beta_local);

            if (value > best) {
                best = value;
                best_pair = {u, v};
            }
        }

        return best_pair;
    }

    auto probs = compute_probabilities(
        candidates, current, g1, g2,
        pheromone, alpha, beta_local
    );

    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double r = dist(rng_);
    double acc = 0.0;

    for (std::size_t i = 0; i < candidates.size(); ++i) {
        acc += probs[i];
        if (r <= acc)
            return candidates[i];
    }

    return candidates.back();
}

std::vector<double> Ant::compute_probabilities(
    const std::vector<std::pair<Vertex, Vertex>>& candidates,
    const Mapping& current,
    const Graph& g1,
    const Graph& g2,
    const PheromoneMatrix& pheromone,
    double alpha,
    double beta_local
) const {
    std::vector<double> values(candidates.size());
    double total = 0.0;

    for (std::size_t i = 0; i < candidates.size(); ++i) {
        auto [u, v] = candidates[i];

        double tau = std::max(pheromone.get(u, v), 1e-6);
        double eta = compute_heuristic(u, v, current, g1, g2);

        values[i] =
            std::pow(tau, alpha) *
            std::pow(eta, beta_local);

        total += values[i];
    }

    if (total > 1e-12) {
        for (auto& x : values) x /= total;
    } else {
        double uniform = 1.0 / values.size();
        std::fill(values.begin(), values.end(), uniform);
    }

    return values;
}

} // namespace mcs
