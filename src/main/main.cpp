#include "../include/graph.hpp"
#include "../include/reader.hpp"
#include "../include/aco.hpp"
#include "../include/graphy.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace mcs;

void print_usage(const char* program_name) {
    std::cout << "Uso: " << program_name << " <gráfica1.txt> <gráfica2.txt> --seed <n> [opciones]\n\n";
    std::cout << "Argumentos obligatorios:\n";
    std::cout << "  <gráfica1.txt>      Archivo de la primera gráfica\n";
    std::cout << "  <gráfica2.txt>      Archivo de la segunda gráfica\n";
    std::cout << "  --seed <n>          Semilla aleatoria (OBLIGATORIO)\n\n";
    std::cout << "Opciones:\n";
    std::cout << "  --ants <n>          Número de hormigas (default: 20)\n";
    std::cout << "  --iterations <n>    Iteraciones máximas (default: 100)\n";
    std::cout << "  --alpha <f>         Peso de feromona (default: 1.0)\n";
    std::cout << "  --beta <f>          Peso de heurística (default: 3.0)\n";
    std::cout << "  --rho <f>           Tasa de evaporación (default: 0.1)\n";
    std::cout << "  --output <nombre>   Generar visualizaciones y exportar\n";
    std::cout << "                      Crea: <nombre>_g1.svg, <nombre>_g2.svg,\n";
    std::cout << "                            <nombre>_solucion.svg, <nombre>.mcis\n";
    std::cout << "  --help, -h          Mostrar esta ayuda\n\n";
    std::cout << "Ejemplos:\n";
    std::cout << "  " << program_name << " g1.txt g2.txt --seed 42\n";
    std::cout << "  " << program_name << " g1.txt g2.txt --seed 123 --output resultado\n";
    std::cout << "      Genera: resultado_g1.svg, resultado_g2.svg,\n";
    std::cout << "              resultado_solucion.svg, resultado.mcis\n";
}

ACOParams parse_arguments(int argc, char* argv[], 
                         std::string& file1, std::string& file2,
                         std::string& output_base) {
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
            }
        } else if (arg == "--iterations") {
            if (i + 1 < argc) {
                params.max_iterations = std::atoi(argv[++i]);
                if (params.max_iterations <= 0) {
                    std::cerr << "Error: --iterations debe ser > 0\n";
                    std::exit(1);
                }
            }
        } else if (arg == "--alpha") {
            if (i + 1 < argc) {
                params.alpha = std::atof(argv[++i]);
            }
        } else if (arg == "--beta") {
            if (i + 1 < argc) {
                params.beta = std::atof(argv[++i]);
            }
        } else if (arg == "--rho") {
            if (i + 1 < argc) {
                params.rho = std::atof(argv[++i]);
                if (params.rho < 0 || params.rho > 1) {
                    std::cerr << "Error: --rho debe estar en [0, 1]\n";
                    std::exit(1);
                }
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                output_base = argv[++i];
            } else {
                std::cerr << "Error: --output requiere un nombre base\n";
                std::exit(1);
            }
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
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << " PARÁMETROS ACO\n";
    std::cout << "═══════════════════════════════════════════════════════════\n";
    std::cout << "   Hormigas:     " << params.num_ants << "\n";
    std::cout << "   Iteraciones:  " << params.max_iterations << "\n";
    std::cout << "   Alpha (τ):    " << std::fixed << std::setprecision(2) << params.alpha << "\n";
    std::cout << "   Beta (η):     " << params.beta << "\n";
    std::cout << "   Rho (evap):   " << params.rho << "\n";
    std::cout << "   Semilla:      " << params.seed << " \n";
    std::cout << "═══════════════════════════════════════════════════════════\n\n";
}

/**
 * @brief Programa principal para encontrar la máxima subgráfica común inducida (MCIS) entre dos gráficas.
 *
 * Lee dos gráficas desde archivos, ejecuta el algoritmo ACO y exporta la solución y visualizaciones.
 */
int main(int argc, char* argv[]) {
    try {
        std::string file1, file2, output_base;
        ACOParams params = parse_arguments(argc, argv, file1, file2, output_base);

        Reader reader1, reader2;
        Graph g1 = reader1.read_graph(file1);
        Graph g2 = reader2.read_graph(file2);

        print_params(params);

        ACO aco(g1, g2, params);
        Mapping solution = aco.solve();

        if (solution.size() == 0) {
            std::cout << "Mapeo: (vacío - no se encontró subgrafica común)\n\n";
            return 0;
        }

        // Mostrar mapeo        
        const auto& names1 = reader1.id_to_name();
        const auto& names2 = reader2.id_to_name();

        // Mostrar mapeo encontrado
        int shown = 0;
        for (const auto& [u, v] : solution.get_nodes_vector()) {
            std::cout << "   " << std::setw(15) << std::left << names1[u]
                      << " → " << names2[v] << "\n";
            shown++;
        }

        /**
         * Exporta siempre la MCIS como archivo, usando el nombre base o uno por defecto.
         */
        std::string mcis_file = output_base.empty() ? "resultado.mcis" : output_base + ".mcis";
        solution.export_mcis(g1, g2, names1, names2, mcis_file);

        /**
         * Genera siempre visualizaciones SVG de ambas gráficas y la solución MCIS.
         * Si no se especifica --output, usa nombres por defecto.
         */
        std::string svg_g1 = output_base.empty() ? "resultado_g1.svg" : output_base + "_g1.svg";
        std::string svg_g2 = output_base.empty() ? "resultado_g2.svg" : output_base + "_g2.svg";
        std::string svg_solucion = output_base.empty() ? "resultado_solucion.svg" : output_base + "_solucion.svg";

        std::vector<int> all_vertices_g1;
        for (int i = 0; i < static_cast<int>(g1.num_vertices()); ++i) {
            all_vertices_g1.push_back(i);
        }
        Graphy viz_g1(g1, reader1.id_to_name());
        viz_g1.dibujaSubgraficaInducidaCircular(all_vertices_g1, svg_g1);

        std::vector<int> all_vertices_g2;
        for (int i = 0; i < static_cast<int>(g2.num_vertices()); ++i) {
            all_vertices_g2.push_back(i);
        }
        Graphy viz_g2(g2, reader2.id_to_name());
        viz_g2.dibujaSubgraficaInducidaCircular(all_vertices_g2, svg_g2);

        std::vector<int> subset_g1;
        std::vector<std::string> combined_names;
        for (const auto& [u, v] : solution.get_nodes_vector()) {
            subset_g1.push_back(u);
            std::string name_u = (u < static_cast<int>(names1.size())) 
                                ? names1[u] : std::to_string(u);
            std::string name_v = (v < static_cast<int>(names2.size())) 
                                ? names2[v] : std::to_string(v);
            combined_names.push_back(name_u + "|" + name_v);
        }
        Graphy viz_solucion(g1, combined_names);
        viz_solucion.dibujaSubgraficaInducidaCircular(subset_g1, svg_solucion);


        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}