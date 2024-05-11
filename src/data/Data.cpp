#include "Data.h"
#include "../CSV.hpp"
#include "../Utils.h"
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <functional>

// Constructors =================================================================================================

std::istringstream Data::prepareCsv(const std::string& path) {
  std::ifstream file(path, std::ios::in | std::ios::binary);
  const auto size = std::filesystem::file_size(path);
  std::string file_contents(size, '\0');
  file.read(file_contents.data(), size);
  std::istringstream input(file_contents);
  return input;
}

bool Data::saveEdge(std::vector<CsvValues> line, Graph<Info>& g) {
  auto orig = line[0].get_int();
  auto dest = line[1].get_int();
  auto dist = line[2].get_flt();
  if (orig.has_value() && dest.has_value() && dist.has_value()) {
    Vertex<Info>* o = g.addVertex(Info(orig.value()));  // addVertex() returns an existing vertex if it already exists
    Vertex<Info>* d = g.addVertex(Info(dest.value()));
    g.addBidirectionalEdge(o, d, dist.value());
    return true;
  } else {
    return false;
  }
}

bool Data::saveNode(std::vector<CsvValues> line, Graph<Info>& g) {
  auto id = line[0].get_int();
  auto longitude = line[1].get_flt();
  auto latitude = line[2].get_flt();
  if (id.has_value() && longitude.has_value() && latitude.has_value()) {
    g.addVertex(Info(id.value(), longitude.value(), latitude.value()));
    return true;
  } else {
    return false;
  }
}

void Data::parseCsv(const std::string& path, Graph<Info>& graph, const std::function<bool(std::vector<CsvValues>, Graph<Info>&)>& saveFn) {
  constexpr auto parser = parse_line().to_fn();
  unsigned num_lines = Utils::countLines(path);
  std::istringstream input = prepareCsv(path);
  auto res = parsum::Result<CsvLine, parsum::ParseError>(CsvLine());
  for (unsigned l = 0; l < num_lines; l++) {
    res = parser(input);
    std::vector<CsvValues> line = res.ok.get_data();
    if (!saveFn(line, graph) && l > 0) error("Failed to parse line " + std::to_string(l) + " in " + path);
    Utils::printLoading(l, num_lines, "Loading " + path);
  }
  Utils::clearLine();
}

Data::Data(const std::string& edge_filename) {
  this->g = Graph<Info>();
  parseCsv(edge_filename, this->g, saveEdge);
}

Data::Data(const std::string& edge_filename, const std::string& node_filename) {
  this->g = Graph<Info>();
  parseCsv(node_filename, this->g, saveNode);
  parseCsv(edge_filename, this->g, saveEdge);
}

Graph<Info> &Data::getGraph() { return g; }

// Functions ====================================================================================================

TSPResult Data::backtracking() {
  // TODO
  error("Not yet implemented");
  return {};
}

TSPResult Data::triangular() {
  // TODO
  error("Not yet implemented");
  return {};
}

TSPResult Data::heuristic() {
  // TODO
  error("Not yet implemented");
  return {};
}

std::optional<TSPResult> Data::disconnected(uint64_t vertexId) {
  // TODO
  error("Not yet implemented");
  return {};
}
