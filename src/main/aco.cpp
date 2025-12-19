#include "../include/aco.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <omp.h>

namespace mcs {

ACO::ACO(const Graph& g1,
         const Graph& g2,
         const ACOParams& params)
    : g1_(g1),
      g2_(g2),
      params_(params),
      pheromone_(g1.num_vertices(), g2.num_vertices(), 1.0),
      best_score_(0) {}


// ===============================================================
// SOLVE
// ===============================================================
Mapping ACO::solve() {
    std::cout << "ACO (MCIS inducido)\n";
    std::cout << "G1 vertices: " << g1_.num_vertices() << "\n";
    std::cout << "G2 vertices: " << g2_.num_vertices() << "\n";
    std::cout << "Hormigas:    " << params_.num_ants << "\n";
    std::cout << "Iteraciones: " << params_.max_iterations << "\n\n";

    for (int iter = 0; iter < params_.max_iterations; ++iter) {
        int prev_best = best_score_;

        run_iteration(iter);

        if (iter % 10 == 0) {
            std::cout << "Iter "
                      << std::setw(6) << iter
                      << " | best_size = "
                      << best_score_
                      << "\n";
        }

        // Parada temprana opcional (si quieres activarla)
        /*
        if (best_score_ == g1_.num_vertices()) {
            std::cout << "\nÓptimo alcanzado.\n";
            break;
        }
        */
    }

    std::cout << "\nFinal: "
              << best_score_
              << " vertices\n";

    return best_solution_;
}


// ===============================================================
// RUN ITERATION (PARALELIZADA POR HORMIGA)
// ===============================================================
void ACO::run_iteration(int iter) {

    std::vector<Mapping> solutions(params_.num_ants);
    std::vector<int>     scores(params_.num_ants, 0);

    int local_best_score = 0;
    Mapping local_best_solution;

    #pragma omp parallel
    {
        int thread_best_score = 0;
        Mapping thread_best_solution;

        // RNG privado por hilo
        std::mt19937 rng(
            params_.seed + iter * 100000 + omp_get_thread_num()
        );

        #pragma omp for schedule(static)
        for (int k = 0; k < params_.num_ants; ++k) {

            Ant ant(rng);

            Mapping sol = ant.construct_solution(
                g1_, g2_,
                pheromone_,
                params_.alpha,
                params_.beta
            );

            int score = sol.size();

            solutions[k] = sol;
            scores[k]    = score;

            if (score > thread_best_score) {
                thread_best_score    = score;
                thread_best_solution = sol;
            }
        }

        #pragma omp critical
        {
            if (thread_best_score > local_best_score) {
                local_best_score    = thread_best_score;
                local_best_solution = thread_best_solution;
            }
        }
    }

    // ===========================================================
    // SECCIÓN SECUENCIAL
    // ===========================================================

    if (local_best_score > best_score_) {
        best_score_    = local_best_score;
        best_solution_ = local_best_solution;
    }

    pheromone_.evaporate(params_.rho);
    update_pheromones(solutions, scores);
}


// ===============================================================
// UPDATE PHEROMONES
// ===============================================================
void ACO::update_pheromones(const std::vector<Mapping>& solutions,
                            const std::vector<int>& scores) {

    int max_score = *std::max_element(scores.begin(), scores.end());
    if (max_score == 0)
        return;

    // Refuerzo proporcional
    for (std::size_t i = 0; i < solutions.size(); ++i) {
        double quality =
            static_cast<double>(scores[i]) / max_score;

        double delta = quality * 5.0;

        for (const auto& [u, v] : solutions[i].get_nodes_vector()) {
            pheromone_.deposit(u, v, delta);
        }
    }

    // Elitismo suave
    if (best_score_ > 0) {
        double elite_bonus = 10.0;
        for (const auto& [u, v] : best_solution_.get_nodes_vector()) {
            pheromone_.deposit(u, v, elite_bonus);
        }
    }
}

} // namespace mcs
