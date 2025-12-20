#include "../include/aco.hpp"
#include "../include/ant.hpp"
#include "../include/mapping.hpp"
#include "../include/pheromone.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace mcs {

ACO::ACO(const Graph& g1, const Graph& g2, const ACOParams& params)
    : g1_(g1),
      g2_(g2),
      params_(params),
      pheromone_(g1.num_vertices(), g2.num_vertices(), 1.0),
      rng_(params.seed),
      best_score_(0),
      best_edges_(0) {}

Mapping ACO::solve() {
    std::cout << "ACO (MCIS inducido)\n";
    std::cout << "G1 vertices: " << g1_.num_vertices() << "\n";
    std::cout << "G2 vertices: " << g2_.num_vertices() << "\n";
    std::cout << "Hormigas: " << params_.num_ants << "\n";
    std::cout << "Iteraciones: " << params_.max_iterations << "\n\n";

    for (int iter = 0; iter < params_.max_iterations; ++iter) {
        run_iteration(iter);
    }

    std::cout << "\nFinal: " << best_score_ << " vertices, " << best_edges_
              << " edges\n";
    return best_solution_;
}

void ACO::run_iteration(int iter) {
    std::vector<Mapping> solutions(params_.num_ants);
    std::vector<int> scores(params_.num_ants, 0);

    int local_best_score = 0;
    int local_best_edges = 0;
    Mapping local_best_solution;

    #pragma omp parallel
    {
        int thread_best_score = 0;
        int thread_best_edges = 0;
        Mapping thread_best_solution;

        #pragma omp for schedule(static)
        for (int k = 0; k < params_.num_ants; ++k) {
            unsigned seed =
                static_cast<unsigned>(params_.seed) ^
                static_cast<unsigned>(iter * 1315423911u) ^
                static_cast<unsigned>(k * 2654435761u);

#ifdef _OPENMP
            seed ^= static_cast<unsigned>(omp_get_thread_num() * 97531u);
#endif

            std::mt19937 local_rng(seed);
            Ant ant(local_rng);

            Mapping sol = ant.construct_solution(
                g1_, g2_, pheromone_, params_.alpha, params_.beta
            );

            const int score = static_cast<int>(sol.size());
            const int edges = sol.count_edges(g1_, g2_);

            solutions[k] = sol;
            scores[k] = score;

            if (score > thread_best_score ||
                (score == thread_best_score && edges > thread_best_edges)) {
                thread_best_score = score;
                thread_best_edges = edges;
                thread_best_solution = sol;
            }
        }

        #pragma omp critical
        {
            if (thread_best_score > local_best_score ||
                (thread_best_score == local_best_score &&
                 thread_best_edges > local_best_edges)) {
                local_best_score = thread_best_score;
                local_best_edges = thread_best_edges;
                local_best_solution = thread_best_solution;
            }
        }
    }

    if (local_best_score > best_score_ ||
        (local_best_score == best_score_ && local_best_edges > best_edges_)) {
        best_score_ = local_best_score;
        best_edges_ = local_best_edges;
        best_solution_ = local_best_solution;
    }

    pheromone_.evaporate(params_.rho);
    update_pheromones(solutions, scores);
}

void ACO::update_pheromones(const std::vector<Mapping>& solutions,
                            const std::vector<int>& scores) {
    const int max_score = *std::max_element(scores.begin(), scores.end());
    if (max_score <= 0) return;

    for (std::size_t i = 0; i < solutions.size(); ++i) {
        if (scores[i] <= 0) continue;

        const double quality = static_cast<double>(scores[i]) / max_score;
        const double delta = 2.0 * quality;

        for (const auto& [u, v] : solutions[i].get_nodes_vector()) {
            pheromone_.deposit(u, v, delta);
        }
    }

    if (best_score_ > 0) {
        const double elite = 5.0;
        for (const auto& [u, v] : best_solution_.get_nodes_vector()) {
            pheromone_.deposit(u, v, elite);
        }
    }
}

} // namespace mcs
