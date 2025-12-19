#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>

namespace mcs {

class Graph;

class Reader {
public:
    Graph read_graph(const std::string& filename);

    const std::vector<std::string>& id_to_name() const { return id_to_name_; }

private:
    int get_or_create_id(const std::string& name);

    bool parse_edge_line(const std::string& line,
                         std::string& u,
                         std::string& v) const;

    std::unordered_map<std::string, int> name_to_id_;
    std::vector<std::string> id_to_name_;
};

} // namespace mcs
