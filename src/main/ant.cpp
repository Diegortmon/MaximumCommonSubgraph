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

// ============================================================================
// FUNCIÓN AUXILIAR: Reparar mapeo a inducido válido
// ============================================================================
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

// ============================================================================
// CONSTRUCCIÓN DE SOLUCIÓN (SIN CUTOFF)
// ============================================================================
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
        if (u == -1) break;  // No hay más candidatos válidos

        // Ya no hay cutoff - is_feasible_add garantiza validez
        mapping.add_pair(u, v);
    }

    // Reparación final (debería ser no-op si todo fue válido)
    return repair_to_induced(mapping, g1, g2);
}

// ============================================================================
// HEURÍSTICA
// ============================================================================
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

// ============================================================================
// SELECCIÓN DE SIGUIENTE PAR (OPTIMIZADO)
// ============================================================================
std::pair<Vertex, Vertex> Ant::select_next_pair(
    const Mapping& current,
    const Graph& g1,
    const Graph& g2,
    const PheromoneMatrix& pheromone,
    double alpha,
    double beta
) {

    // Parámetros de muestreo
    const int K = 1000;              // Candidatos finales
    const int MAX_ATTEMPTS = K * 3; // Intentos de muestreo

    // Construir listas de nodos no mapeados
    std::vector<Vertex> U, V;
    U.reserve(g1.num_vertices());
    V.reserve(g2.num_vertices());

    for (Vertex u = 0; u < static_cast<Vertex>(g1.num_vertices()); ++u)
        if (!current.is_mapped_g1(u)) U.push_back(u);

    for (Vertex v = 0; v < static_cast<Vertex>(g2.num_vertices()); ++v)
        if (!current.is_mapped_g2(v)) V.push_back(v);

    if (U.empty() || V.empty()) return {-1, -1};

    std::uniform_int_distribution<int> du(0, static_cast<int>(U.size()) - 1);
    std::uniform_int_distribution<int> dv(0, static_cast<int>(V.size()) - 1);

    // Pool de candidatos válidos
    struct Cand { 
        Vertex u, v; 
    };
    std::vector<Cand> pool;
    pool.reserve(K);

    // ========================================================================
    // FASE 1: Muestreo con filtros
    // ========================================================================
    int attempts = 0;
    while (static_cast<int>(pool.size()) < K && attempts < MAX_ATTEMPTS) {
        Vertex u = U[du(rng_)];
        Vertex v = V[dv(rng_)];

        // Pre-filtro: diferencia de grados
        int deg_diff = std::abs(g1.degree(u) - g2.degree(v));
        if (deg_diff > 5) {
            ++attempts;
            continue;
        }

        // Filtro completo: factibilidad MCIS inducido
        if (!current.is_feasible_add(u, v, g1, g2)) {
            ++attempts;
            continue;
        }

        pool.push_back({u, v});
        ++attempts;
    }

    if (pool.empty()) return {-1, -1};

    // ========================================================================
    // FASE 2: Calcular probabilidades ACO
    // ========================================================================
    std::vector<double> values(pool.size());
    double total = 0.0;

    for (std::size_t i = 0; i < pool.size(); ++i) {
        Vertex u = pool[i].u;
        Vertex v = pool[i].v;

        double tau = std::max(pheromone.get(u, v), 1e-6);
        double eta = compute_heuristic(u, v, current, g1, g2);

        double val = std::pow(tau, alpha) * std::pow(eta, beta);
        values[i] = val;
        total += val;
    }

    // ========================================================================
    // FASE 3: Selección probabilística (ruleta)
    // ========================================================================
    if (total <= 1e-12) {
        // Fallback: selección uniforme
        std::uniform_int_distribution<int> pick(0, static_cast<int>(pool.size()) - 1);
        auto c = pool[pick(rng_)];
        return {c.u, c.v};
    }

    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double r = dist(rng_);
    double acc = 0.0;

    for (std::size_t i = 0; i < pool.size(); ++i) {
        acc += values[i] / total;
        if (r <= acc) {
            return {pool[i].u, pool[i].v};
        }
    }

    return {pool.back().u, pool.back().v};
}

} // namespace mcs