#pragma once
#include "graph.hpp"
#include "mapping.hpp"
#include "pheromone.hpp"
#include "ant.hpp"
#include <vector>
#include <random>

namespace mcs {

/**
 * @brief Parámetros configurables del algoritmo de Colonia de Hormigas (ACO).
 *
 * Contiene los parámetros que controlan el comportamiento de la metaheurística:
 * - num_ants: número de hormigas que construyen soluciones por iteración.
 * - max_iterations: número máximo de iteraciones del algoritmo.
 * - alpha: peso de la feromona (influencia en la probabilidad de selección).
 * - beta: peso de la heurística (influencia de la información heurística).
 * - rho: tasa de evaporación de la feromona (valores en [0,1]).
 * - seed: semilla para el generador de números aleatorios.
 */
struct ACOParams {
    int num_ants = 14;
    int max_iterations = 1000;
    double alpha = 1.0;
    double beta = 3.0;
    double rho = 0.1;
    int seed = 42;
};

/**
 * @brief Implementación del algoritmo ACO para el problema MCIS (Maximum Common Induced Subgraph).
 *
 * La clase encapsula el estado necesario (graficas, parámetros, matriz de feromonas, RNG)
 * y proporciona la interfaz pública para ejecutar la búsqueda y obtener la mejor solución.
 */
class ACO {
public:
    /**
     * @brief Construye el objeto ACO con los graficas y parámetros provistos.
     *
     * @param g1 Referencia al primer grafica (estructura usada como base para mapeos).
     * @param g2 Referencia al segundo grafica.
     * @param params Parámetros de ejecución (número de hormigas, alfa, beta, etc.).
     */
    ACO(const Graph& g1, const Graph& g2, const ACOParams& params);

    /**
     * @brief Ejecuta el algoritmo ACO y devuelve la mejor asignación (Mapping) encontrada.
     *
     * El método ejecuta hasta params_.max_iterations iteraciones (o criterio de parada interno)
     * y actualiza internamente best_solution_.
     *
     * @return Mapping mejor mapeo encontrado entre vértices de g1 y g2.
     */
    Mapping solve();

    /**
     * @brief Obtiene la mejor solución encontrada hasta el momento.
     *
     * @return Referencia constante a la Mapping interna que contiene el mejor mapeo.
     */
    const Mapping& get_best_solution() const { return best_solution_; }

    /**
     * @brief Historial de convergencia (por ejemplo puntuación por iteración).
     *
     * Se expone para fines de análisis/plotting.
     */
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

    /**
     * @brief Ejecuta una iteración completa del algoritmo (construcción de soluciones y actualización local).
     *
     * @param iter Índice (0-based) de la iteración actual.
     */
    void run_iteration(int iter);

    /**
     * @brief Actualiza la matriz de feromonas en base a las soluciones generadas en la iteración.
     *
     * Este método aplica evaporación y refuerzo proporcional a la calidad de cada solución.
     *
     * @param solutions Vector con las mappings construidas por las hormigas.
     * @param scores Vector con la calidad (puntuación) correspondiente a cada mapping.
     */
    void update_pheromones(const std::vector<Mapping>& solutions,
                          const std::vector<int>& scores);
};

} // namespace mcs