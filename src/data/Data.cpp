#include "Data.h"
#include "../Utils.h"
#include "Graph.hpp"
#include <cfloat>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

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
    g.addVertex(Info(id.value(), longitude.value(), latitude.value()),
                id.value());
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

double calc_weight(Graph<Info> &root, uint64_t src, uint64_t dst) {
  auto &vertex_set = root.getVertexSet();
  Edge<Info> *ed = root.findEdge(src, dst);
  double weight = 0;
  if (ed != nullptr) {
    weight = ed->getWeight();
  } else {
    weight = vertex_set[src].getInfo().distance(vertex_set[dst].getInfo());
  }
  return weight;
}

TSPResult heuristic_impl(Graph<Info> &root) {
  double cost = 0;
  double min = DBL_MAX;
  uint64_t selected = 0;
  auto &vertex_set = root.getVertexSet();
  std::vector<uint64_t> path;
  path.reserve(root.getVertexSet().size() + 1);
  path.push_back(0);
  vertex_set[0].setProcessing(true);
  for (uint64_t i = 0; i < vertex_set.size() - 1; ++i) {
    for (uint64_t j = 0; j < vertex_set.size(); ++j) {
      if (vertex_set[j].isProcessing())
        continue;
      if (j == path.back())
        continue;
      double weight = calc_weight(root, path.back(), j);
      if (weight < min) {
        min = weight;
        selected = j;
      }
    }
    path.push_back(selected);
    vertex_set[selected].setProcessing(true);
    cost += min;
    min = DBL_MAX;
  }
  cost += calc_weight(root, path.back(), 0);
  path.push_back(0);
  for (uint64_t i = 0; i < vertex_set.size(); ++i) {
    vertex_set[i].setProcessing(false);
  }
  return {cost, path};
}

TSPResult Data::heuristic() {
  // TODO
  // error("Not yet implemented");
  return heuristic_impl(this->g);
}

// ====================================================================================================

#define ALPHA 0.9
#define BETA 1.5
#define EXPLORATION_CONSTANT 0.0001
#define HYPERPARAMETER 0.1
#define DEGREDACTION_RATE 0.1
#define DEFAULT_PHEROMONE 0.1

void updatePheromoneLevels(Graph<Info> &g, TSPResult &result) {
  double pheromone = HYPERPARAMETER / result.cost * DEGREDACTION_RATE;
  for (int i = 0; i < result.path.size() - 1; ++i) {
    Edge<Info> *e = g.findEdge(result.path[i], result.path[i + 1]);
    e->setFlow(e->getFlow() + pheromone);
  }
}

TSPResult traverseGraphUsingAnts(Graph<Info> &g, Vertex<Info> &start) {
  for (auto& [_, i]: g.getVertexSet()) i.setVisited(false);
  TSPResult result = {0, {start.getId()}};
  uint64_t currentId = start.getId();

  // Loop through all vertices
  for (int steps = 0; steps < g.getNumVertex(); ++steps) {
    Vertex<Info> &currentNode = g.findVertex(currentId);
    currentNode.setVisited(true);
    std::vector<std::reference_wrapper<const Edge<Info>>> possibleEdges;
    std::vector<double> probabilities;

    // Calculate probabilities for each edge
    for (const auto &[dest, e]: currentNode.getAdj()) {
      // Ignore unwanted edges
      if (g.findVertex(dest).isVisited()) {
        if (dest == start.getId() && steps == g.getNumVertex() - 1) {
          possibleEdges.push_back(std::ref(e));
          probabilities.push_back(1);
          break;
        } else continue;
      }

      // Calculate probability
      double pheromoneLevel = fmax(e.getFlow(), EXPLORATION_CONSTANT);
      double probability = pow(pheromoneLevel, ALPHA) / pow(e.getWeight(), BETA);
      possibleEdges.push_back(std::ref(e));
      probabilities.push_back(probability);
    }

    if (possibleEdges.empty()) { // No possible edges
      updatePheromoneLevels(g, result);
      return {DBL_MAX, result.path};
    }

    // Select edge
    auto& edgeSelected = Utils::weightedRandomElement(possibleEdges, probabilities).get();
    currentId = edgeSelected.getDest();
    result.cost += edgeSelected.getWeight();
    result.path.push_back(currentId);
  }

  updatePheromoneLevels(g, result);
  return result;
}

std::optional<TSPResult> Data::disconnected(uint64_t vertexId, unsigned iterations) {
  // Set default values
  for (auto& [_, v]: g.getVertexSet()) {
    v.setVisited(false);
    for (auto& [_, e]: v.getAdj())
      e.setFlow(DEFAULT_PHEROMONE);
  }

  Vertex<Info> &v = g.findVertex(vertexId);
  TSPResult bestResult = {DBL_MAX, {}};
  for (int i = 0; i < iterations; ++i) {
    TSPResult res = traverseGraphUsingAnts(g, v);
    std::cout << "Iteration " << i << " : " << res.cost;
    if (res < bestResult) {
      bestResult = res;
      std::cout << " [*]";
    }
    std::cout << "               \r";
    std::cout.flush();
  }

  if (bestResult.cost == DBL_MAX) return {};
  return bestResult;
}
