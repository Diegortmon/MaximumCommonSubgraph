#include "../include/ant.hpp"
#include "../include/graph.hpp"
#include "../include/mapping.hpp"
#include "../include/pheromone.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <utility>
#include <vector>

namespace mcs {

static inline int conflict_count(Vertex u, Vertex v,
                                 const Mapping& current,
                                 const Graph& g1,
                                 const Graph& g2,
                                 int cutoff) {
    int mism = 0;
    for (const auto& [um, vm] : current.get_nodes_vector()) {
        const bool e1 = g1.edge(u, um);
        const bool e2 = g2.edge(v, vm);
        if (e1 != e2) {
            if (++mism >= cutoff) return mism;
        }
    }
    return mism;
}

static inline Mapping repair_to_induced(const Mapping& in,
                                       const Graph& g1,
                                       const Graph& g2) {
    Mapping out;
    for (const auto& [u, v] : in.get_nodes_vector()) {
        if (out.is_mapped_g1(u) || out.is_mapped_g2(v)) continue;
        if (out.is_feasible_add(u, v, g1, g2)) out.add_pair(u, v);
    }
    return out;
}

Mapping Ant::construct_solution(const Graph& g1,
                               const Graph& g2,
                               const PheromoneMatrix& pheromone,
                               double alpha,
                               double beta) {
    Mapping mapping;
    const int max_steps =
        static_cast<int>(std::min(g1.num_vertices(), g2.num_vertices()));

    for (int step = 0; step < max_steps; ++step) {
        auto [u, v] = select_next_pair(mapping, g1, g2, pheromone, alpha, beta);
        if (u == -1) break;

        const int cutoff = (mapping.size() < 25) ? 2 : 1;
        if (conflict_count(u, v, mapping, g1, g2, cutoff) >= cutoff) continue;

        mapping.add_pair(u, v);
    }

    return repair_to_induced(mapping, g1, g2);
}

double Ant::compute_heuristic(Vertex u,
                              Vertex v,
                              const Mapping& current,
                              const Graph& g1,
                              const Graph& g2) const {
    const int deg_u = g1.degree(u);
    const int deg_v = g2.degree(v);
    const int diff = std::abs(deg_u - deg_v);
    const double deg_sim = 1.0 / (1.0 + diff);

    if (current.size() == 0) return (deg_u + deg_v + 1.0) * deg_sim;

    int preserved = 0;
    int mism = 0;
    int checked = 0;

    for (const auto& [um, vm] : current.get_nodes_vector()) {
        const bool e1 = g1.edge(u, um);
        const bool e2 = g2.edge(v, vm);
        ++checked;
        if (e1 && e2) ++preserved;
        if (e1 != e2) ++mism;
    }

    const double density =
        static_cast<double>(preserved) / std::max(1, checked);

    const double lambda = 2.0;
    const double soft = 1.0 / (1.0 + lambda * mism);

    return (preserved + 1.0) * (1.0 + density) * soft
           + 0.05 * (deg_u + deg_v) * deg_sim;
}

std::pair<Vertex, Vertex> Ant::select_next_pair(const Mapping& current,
                                                const Graph& g1,
                                                const Graph& g2,
                                                const PheromoneMatrix& pheromone,
                                                double alpha,
                                                double beta) {
    const int n1 = static_cast<int>(g1.num_vertices());
    const int n2 = static_cast<int>(g2.num_vertices());
    if (current.size() >= std::min(n1, n2)) return {-1, -1};

    constexpr int MAX_CAND = 2048;
    const int tries = std::min(50000, n1 * 50);

    std::vector<std::pair<Vertex, Vertex>> candidates;
    candidates.reserve(MAX_CAND);

    std::uniform_int_distribution<int> du(0, n1 - 1);
    std::uniform_int_distribution<int> dv(0, n2 - 1);

    const int base_tol = (current.size() < 10) ? 8 : 4;
    const int cutoff = (current.size() < 25) ? 2 : 1;

    for (int t = 0; t < tries && static_cast<int>(candidates.size()) < MAX_CAND; ++t) {
        Vertex u = du(rng_);
        if (current.is_mapped_g1(u)) continue;

        Vertex v = dv(rng_);
        if (current.is_mapped_g2(v)) continue;

        if (std::abs(g1.degree(u) - g2.degree(v)) > base_tol) continue;

        if (conflict_count(u, v, current, g1, g2, cutoff) >= cutoff) continue;

        candidates.emplace_back(u, v);
    }

    if (candidates.empty()) return {-1, -1};

    double best = -1.0;
    std::pair<Vertex, Vertex> best_pair = candidates.front();

    double total = 0.0;
    std::vector<double> weights;
    weights.reserve(candidates.size());

    for (const auto& [u, v] : candidates) {
        const double tau = std::max(pheromone.get(u, v), 1e-8);
        const double eta = compute_heuristic(u, v, current, g1, g2);
        const double w = std::pow(tau, alpha) * std::pow(eta, beta);
        weights.push_back(w);
        total += w;
        if (w > best) {
            best = w;
            best_pair = {u, v};
        }
    }

    if (candidates.size() <= 4 || total <= 1e-15) return best_pair;

    std::uniform_real_distribution<double> dist(0.0, total);
    double r = dist(rng_);
    double acc = 0.0;
    for (std::size_t i = 0; i < candidates.size(); ++i) {
        acc += weights[i];
        if (r <= acc) return candidates[i];
    }
    return candidates.back();
}

} // namespace mcs
