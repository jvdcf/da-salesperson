#include "Data.h"
#include "../Utils.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

// Constructors
// =================================================================================================

std::istringstream Data::prepareCsv(const std::string &path) {
  std::ifstream file(path, std::ios::in | std::ios::binary);
  const auto size = std::filesystem::file_size(path);
  std::string file_contents(size, '\0');
  file.read(file_contents.data(), size);
  std::istringstream input(file_contents);
  return input;
}

bool Data::saveEdge(std::vector<CsvValues> const &line, Graph<Info> &g) {
  auto orig = line[0].get_int();
  auto dest = line[1].get_int();
  auto dist = line[2].get_flt();
  if (orig.has_value() && dest.has_value() && dist.has_value()) {
    uint64_t orig_id = orig.value();
    uint64_t dest_id = dest.value();
    double distance = dist.value();
    Vertex<Info> &o = g.findOrAddVertex(orig_id, Info(orig_id));
    Vertex<Info> &d = g.findOrAddVertex(dest_id, Info(dest_id));
    g.addBidirectionalEdge(o, d, distance);
    return true;
  } else {
    return false;
  }
}

bool Data::saveNode(std::vector<CsvValues> const &line, Graph<Info> &g) {
  auto id = line[0].get_int();
  auto longitude = line[1].get_flt();
  auto latitude = line[2].get_flt();
  if (id.has_value() && longitude.has_value() && latitude.has_value()) {
    g.addVertex(Info(id.value(), longitude.value(), latitude.value()), id.value());
    return true;
  } else {
    return false;
  }
}

void Data::parseCsv(const std::string &path, Graph<Info> &graph,
                    const savefn_t saveFn) {
  constexpr auto parser = parse_line().to_fn();
  unsigned num_lines = Utils::countLines(path);
  std::istringstream input = prepareCsv(path);
  auto res = parsum::Result<CsvLine, parsum::ParseError>(CsvLine());
  uint64_t l = 0;
  while ((res = parser(input)).has_val) {
    std::vector<CsvValues> line = res.ok.get_data();
    l++;
    if (!saveFn(line, graph) && l > 1)
      error("Failed to parse line " + std::to_string(l) + " in " + path);
    Utils::printLoading(l, num_lines, "Loading " + path);
  }
  Utils::clearLine();
}

Data::Data(const std::string &edge_filename) {
  this->g = Graph<Info>();
  parseCsv(edge_filename, this->g, saveEdge);
}

Data::Data(const std::string &edge_filename, const std::string &node_filename) {
  this->g = Graph<Info>();
  parseCsv(node_filename, this->g, saveNode);
  parseCsv(edge_filename, this->g, saveEdge);
}

Graph<Info> &Data::getGraph() { return g; }

// Functions
// ====================================================================================================

TSPResult Data::backtracking() {
  // TODO
  error("Not yet implemented");
  return {};
}

TSPResult Data::triangular() {
  // Prim's algorithm - Minimum Spanning Tree
  Utils::prim(&g);

  // DFS - Depth First Search in the MST
  std::vector<uint64_t> dfs = Utils::MSTdfs(&g);

  // Calculate the cost and the path
  double totalCost = 0;
  std::vector<Info> path;

  for (int i=0; i<dfs.size()-1; i++){
    double cost = Utils::weight(dfs[i], dfs[i+1], &g);
    totalCost += cost;
    path.push_back(g.findVertex(dfs[i]).getInfo());
  }

  // Add last vertex to path
  path.push_back(g.findVertex(dfs[dfs.size()-1]).getInfo());

  // Deal with the last edge (returning to the beginning)
  totalCost +=  Utils::weight(dfs[dfs.size()-1], dfs[0], &g);

  // Add the first vertex to the end of the path
  path.push_back(g.findVertex(dfs[0]).getInfo());

  return TSPResult{path, totalCost};
}

TSPResult Data::heuristic() {
  // TODO
  error("Not yet implemented");
  return {};
}

std::optional<TSPResult> Data::disconnected(uint64_t vertexId) {
  // TODO
  error("Not yet implemented");

  // Check if the graph is connected
  if (!Utils::isConnected(&g)) {
    std::cout << "Graph is not connected" << std::endl;
    return {};
  }

  // Find a hamiltonian path


  // nearest neighbor
  return {};
}
