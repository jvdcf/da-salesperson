#include "Data.h"
#include "../Utils.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>
#include <cfloat>
#include <valarray>

// Constructors
// ====================================================================================================

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

std::vector<std::reference_wrapper<const Edge<Info>>>
generatePossibleEdges(Graph<Info> &g, Vertex<Info> &v, const std::vector<uint64_t> &path) {
  std::vector<std::reference_wrapper<const Edge<Info>>> possibleEdges;
  if (path.size() == g.getNumVertex() - 1) {  // If all vertices have been visited, add edge to start
    for (const auto &e: v.getAdj()) {
      if (e.first == START_VERTEX) {
        possibleEdges.push_back(std::ref(e.second));
        break;
      }
    }
  } else {
    for (const auto &e: v.getAdj()) { // Add all edges that their destination vertex hasn't been visited yet
      if (e.first == START_VERTEX) continue;
      if (std::find(path.begin(), path.end(), e.first) != path.end()) continue;
      possibleEdges.push_back(std::ref(e.second));
    }
  }
  return possibleEdges;
}

TSPResult btDFS(Graph<Info> &g, const TSPResult &p, Vertex<Info> &v, double &bestCost) {
  auto possibleEdges = generatePossibleEdges(g, v, p.path);
  TSPResult bestResult = {DBL_MAX, {}};

  // Base cases
  if (p.path.size() == g.getNumVertex()) {
    if (p.cost < bestCost) bestCost = p.cost;
    return {p.cost, p.path};    // Hamiltonian cycle complete
  }

  // Bounding
  if (p.cost >= bestCost) return bestResult;

  // Generate results and pick the best one
  for (std::reference_wrapper<const Edge<Info>> e: possibleEdges) {
    double nextCost = p.cost + e.get().getWeight();
    auto nextPath = p.path;
    Vertex<Info> &nextVertex = g.findVertex(e.get().getDest());
    nextPath.push_back(nextVertex.getId());
    TSPResult next = {nextCost, nextPath};
    auto result = btDFS(g, next, nextVertex, bestCost);
    if (result < bestResult) bestResult = result;
  }

  return bestResult;
}

TSPResult Data::backtracking() {
  Vertex<Info> &start = g.findVertex(START_VERTEX);
  TSPResult p = {0, {}};
  auto bestCost = DBL_MAX;

  TSPResult res = btDFS(g, p, start, bestCost);
  res.path.insert(res.path.begin(), START_VERTEX);
  return res;
}

// ====================================================================================================

TSPResult Data::triangular() {
  // TODO
  error("Not yet implemented");
  return {};
}

// ====================================================================================================

#define DEFAULT_PHEROMONE 0.1
#define ALPHA 0.9
#define BETA 1.5
#define ITERATIONS 100

void antWalk(Graph<Info> &g, Vertex<Info> &v, std::vector<std::vector<double>> &matrixOfPheromones) {
  std::vector<uint64_t> path;
  double cost = 0;
  path.push_back(v.getId());
  v.setVisited(true);
  for (int _ = 0; _ < g.getNumVertex() - 1; _++) {
    std::vector<std::reference_wrapper<const Edge<Info>>> possibleEdges = generatePossibleEdges(g, v, path);
    double sum = 0;
    for (const auto &e: possibleEdges) {
      sum += pow(matrixOfPheromones[v.getId()][e.get().getDest()], ALPHA) * pow(1 / e.get().getWeight(), BETA);
    }
    double r = (double) rand() / RAND_MAX;
    double partialSum = 0;
    for (const auto &e: possibleEdges) {
      double probability = (pow(matrixOfPheromones[v.getId()][e.get().getDest()], ALPHA) * pow(1 / e.get().getWeight(), BETA)) / sum;
      partialSum += probability;
      if (r <= partialSum) {
        cost += e.get().getWeight();
        path.push_back(e.get().getDest());
        v = g.findVertex(e.get().getDest());
        v.setVisited(true);
        break;
      }
    }
  }
  for (auto &vertex: g.getVertexSet()) {
    vertex.second.setVisited(false);
  }
}

TSPResult dfsPheromones(Graph<Info> &g, Vertex<Info> &v, std::vector<std::vector<double>> &matrixOfPheromones) {
  TSPResult bestResult = {DBL_MAX, {}};
  for (int _ = 0; _ < ITERATIONS; _++) {
    antWalk(g, v, matrixOfPheromones);
    TSPResult result = {0, {}};
    for (int i = 0; i < g.getNumVertex(); i++) {
      for (int j = 0; j < g.getNumVertex(); j++) {
        if (matrixOfPheromones[i][j] != DEFAULT_PHEROMONE) {
          result.cost += Utils::weight(i, j, g);
          result.path.push_back(i);
        }
      }
    }
    if (result < bestResult) bestResult = result;
  }
  return bestResult;
}

TSPResult Data::heuristic() {
  Vertex<Info> &v = g.findVertex(START_VERTEX);
  auto matrixOfPheromones = std::vector<std::vector<double>>(g.getNumVertex(), std::vector<double>(g.getNumVertex(), DEFAULT_PHEROMONE));
  for (auto& [_, vertex]: g.getVertexSet()) vertex.setVisited(false);
  for (int _ = 0; _ < 100; _++) antWalk(g, v, matrixOfPheromones);
  return dfsPheromones(g, v, matrixOfPheromones);
}

std::optional<TSPResult> Data::disconnected(uint64_t vertexId) {
  // TODO
  error("Not yet implemented");
  return {};
}
