#include "../include/graph.hpp"
#include "../include/reader.hpp"
#include "../include/aco.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace mcs;

void print_usage(const char* program_name) {
    std::cout << "Uso: " << program_name << " <grafo1.txt> <grafo2.txt> --seed <n> [opciones]\n\n";
    std::cout << "Argumentos obligatorios:\n";
    std::cout << "  <grafo1.txt>        Archivo del primer grafo\n";
    std::cout << "  <grafo2.txt>        Archivo del segundo grafo\n";
    std::cout << "  --seed <n>          Semilla aleatoria (OBLIGATORIO)\n\n";
    std::cout << "Opciones:\n";
    std::cout << "  --ants <n>          Número de hormigas (default: 20)\n";
    std::cout << "  --iterations <n>    Iteraciones máximas (default: 100)\n";
    std::cout << "  --alpha <f>         Peso de feromona (default: 1.0)\n";
    std::cout << "  --beta <f>          Peso de heurística (default: 3.0)\n";
    std::cout << "  --rho <f>           Tasa de evaporación (default: 0.1)\n";
    std::cout << "  --help, -h          Mostrar esta ayuda\n\n";
    std::cout << "Ejemplos:\n";
    std::cout << "  " << program_name << " g1.txt g2.txt --seed 42\n";
    std::cout << "  " << program_name << " g1.txt g2.txt --seed 123 --ants 50 --iterations 200\n";
    std::cout << "  " << program_name << " g1.txt g2.txt --seed 999 --alpha 1.5 --beta 4.0 --rho 0.15\n\n";
    std::cout << "Rangos recomendados:\n";
    std::cout << "  ants:       10-100\n";
    std::cout << "  iterations: 50-500\n";
    std::cout << "  alpha:      0.5-3.0\n";
    std::cout << "  beta:       1.0-5.0\n";
    std::cout << "  rho:        0.05-0.5\n";
}

ACOParams parse_arguments(int argc, char* argv[], std::string& file1, std::string& file2) {
    ACOParams params;
    bool seed_provided = false;

    if (argc < 3) {
        print_usage(argv[0]);
        std::exit(1);
    }

    file1 = argv[1];
    file2 = argv[2];

    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            std::exit(0);
        } else if (arg == "--seed") {
            if (i + 1 < argc) {
                params.seed = std::atoi(argv[++i]);
                seed_provided = true;
            } else {
                std::cerr << "Error: --seed requiere un valor\n";
                std::exit(1);
            }
        } else if (arg == "--ants") {
            if (i + 1 < argc) {
                params.num_ants = std::atoi(argv[++i]);
                if (params.num_ants <= 0) {
                    std::cerr << "Error: --ants debe ser > 0\n";
                    std::exit(1);
                }
            } else {
                std::cerr << "Error: --ants requiere un valor\n";
                std::exit(1);
            }
        } else if (arg == "--iterations") {
            if (i + 1 < argc) {
                params.max_iterations = std::atoi(argv[++i]);
                if (params.max_iterations <= 0) {
                    std::cerr << "Error: --iterations debe ser > 0\n";
                    std::exit(1);
                }
            } else {
                std::cerr << "Error: --iterations requiere un valor\n";
                std::exit(1);
            }
        } else if (arg == "--alpha") {
            if (i + 1 < argc) {
                params.alpha = std::atof(argv[++i]);
                if (params.alpha < 0) {
                    std::cerr << "Error: --alpha debe ser >= 0\n";
                    std::exit(1);
                }
            } else {
                std::cerr << "Error: --alpha requiere un valor\n";
                std::exit(1);
            }
        } else if (arg == "--beta") {
            if (i + 1 < argc) {
                params.beta = std::atof(argv[++i]);
                if (params.beta < 0) {
                    std::cerr << "Error: --beta debe ser >= 0\n";
                    std::exit(1);
                }
            } else {
                std::cerr << "Error: --beta requiere un valor\n";
                std::exit(1);
            }
        } else if (arg == "--rho") {
            if (i + 1 < argc) {
                params.rho = std::atof(argv[++i]);
                if (params.rho < 0 || params.rho > 1) {
                    std::cerr << "Error: --rho debe estar en [0, 1]\n";
                    std::exit(1);
                }
            } else {
                std::cerr << "Error: --rho requiere un valor\n";
                std::exit(1);
            }
        } else {
            std::cerr << "Warning: argumento desconocido '" << arg << "' (ignorado)\n";
        }
    }

    if (!seed_provided) {
        std::cerr << "Error: --seed es obligatorio\n\n";
        print_usage(argv[0]);
        std::exit(1);
    }

    return params;
}

void print_params(const ACOParams& params) {
    std::cout << " Parámetros ACO:\n";
    std::cout << "   Hormigas:     " << params.num_ants << "\n";
    std::cout << "   Iteraciones:  " << params.max_iterations << "\n";
    std::cout << "   Alpha (τ):    " << std::fixed << std::setprecision(2) << params.alpha << "\n";
    std::cout << "   Beta (η):     " << params.beta << "\n";
    std::cout << "   Rho (evap):   " << params.rho << "\n";
    std::cout << "   Semilla:      " << params.seed << " ⭐\n\n";
}

int main(int argc, char* argv[]) {
    try {
        std::string file1, file2;
        ACOParams params = parse_arguments(argc, argv, file1, file2);

        std::cout << "Leyendo grafos...\n";
        Reader reader1, reader2;
        Graph g1 = reader1.read_graph(file1);
        Graph g2 = reader2.read_graph(file2);

        std::cout << "   G1: " << g1.num_vertices() << " vértices (desde " << file1 << ")\n";
        std::cout << "   G2: " << g2.num_vertices() << " vértices (desde " << file2 << ")\n\n";

        print_params(params);

        std::cout << "Ejecutando ACO para MCIS...\n";
        std::cout << std::string(60, '=') << "\n\n";

        ACO aco(g1, g2, params);
        Mapping solution = aco.solve();

        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "RESULTADO FINAL\n";
        std::cout << std::string(60, '=') << "\n\n";

        std::cout << "Estadísticas:\n";
        std::cout << "   Nodos mapeados: " << solution.size() << "\n";
        std::cout << "   Aristas:        " << solution.count_edges(g1, g2) << "\n";
        std::cout << "   Factible:       ok (por construcción)\n\n";

        std::cout << "Mapeo encontrado:\n";
        if (solution.size() == 0) {
            std::cout << "   (vacío - no se encontró subgrafo común)\n";
        } else {
            const auto& names1 = reader1.id_to_name();
            const auto& names2 = reader2.id_to_name();

            for (const auto& [u, v] : solution.get_nodes_vector()) {
                std::cout << "   " << std::setw(15) << std::left << names1[u]
                          << " → " << names2[v] << "\n";
            }
        }

        std::cout << "\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
