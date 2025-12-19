#include "../include/reader.hpp"
#include "../include/graph.hpp"
#include <iostream>
#include <stdexcept>

namespace mcs {

Graph Reader::read_graph(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("No se puede abrir: " + filename);
    }

    name_to_id_.clear();
    id_to_name_.clear();

    std::vector<std::pair<std::string, std::string>> edges;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::string u, v;
        if (parse_edge_line(line, u, v)) {
            edges.emplace_back(u, v);
        }
    }

    // Crear grafo
    Graph g(id_to_name_.size());
    for (const auto& [u_name, v_name] : edges) {
        int u = name_to_id_[u_name];
        int v = name_to_id_[v_name];
        g.add_edge(u, v);
    }

    return g;
}

int Reader::get_or_create_id(const std::string& name) {
    auto it = name_to_id_.find(name);
    if (it != name_to_id_.end()) {
        return it->second;
    }

    int id = static_cast<int>(id_to_name_.size());
    name_to_id_[name] = id;
    id_to_name_.push_back(name);
    return id;
}

bool Reader::parse_edge_line(const std::string& line,
                             std::string& u,
                             std::string& v) const {
    std::istringstream iss(line);
    std::string token;

    if (!(iss >> u >> v)) {
        return false;
    }

    // Quitar ';' si existe
    if (!v.empty() && v.back() == ';') {
        v.pop_back();
    }

    // Crear IDs si no existen
    const_cast<Reader*>(this)->get_or_create_id(u);
    const_cast<Reader*>(this)->get_or_create_id(v);

    return true;
}
} // namespace mcs