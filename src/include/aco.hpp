#pragma once
#include "graph.hpp"
#include "mapping.hpp"
#include "pheromone.hpp"
#include "ant.hpp"
#include <vector>
#include <random>

namespace mcs {

struct ACOParams {
    int num_ants = 14;
    int max_iterations = 1000;
    double alpha = 1.0;
    double beta = 3.0;
    double rho = 0.1;
    int seed = 42;
};

class ACO {
public:
    ACO(const Graph& g1, const Graph& g2, const ACOParams& params);

    Mapping solve();

    const Mapping& get_best_solution() const { return best_solution_; }
    const std::vector<int>& get_convergence_history() const {
        return convergence_history_;
    }

private:
    const Graph& g1_;
    const Graph& g2_;
    ACOParams params_;

    PheromoneMatrix pheromone_;
    std::mt19937 rng_;

    Mapping best_solution_;
    int best_score_;
    int best_edges_;

    std::vector<int> convergence_history_;

    void run_iteration(int iter);
    void update_pheromones(const std::vector<Mapping>& solutions,
                          const std::vector<int>& scores);
};

} // namespace mcs