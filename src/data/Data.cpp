#include "Data.h"
#include "../Utils.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>

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
    Vertex<Info> *o =
        g.addVertex(Info(orig.value())); // addVertex() returns an existing
                                         // vertex if it already exists
    Vertex<Info> *d = g.addVertex(Info(dest.value()));
    g.addBidirectionalEdge(o, d, dist.value());
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
    g.addVertex(Info(id.value(), longitude.value(), latitude.value()));
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

struct BTPossibility {
  double cost;                            // Current cost of all edges selected until now
  std::queue<Edge<Info>*> possibleEdges;  // Edges that can be selected if this edge is ignored (the first edge is the one being considered)
  std::vector<Info> path;                 // Path of vertices selected until now
};

std::queue<Edge<Info>*> generatePossibleEdges(Graph<Info>& g, Vertex<Info>* v, const std::vector<Info>& path) {
  std::queue<Edge<Info> *> possibleEdges;
  if (path.size() == g.getNumVertex()) {  // If all vertices have been visited, add edge to start
    for (Edge<Info> *e: v->getAdj()) {
      if (e->getDest()->getInfo().getId() == START_VERTEX) {
        possibleEdges.push(e);
        break;
      }
    }
  } else {
    for (Edge<Info> *e: v->getAdj()) { // Add all edges that their destination vertex hasn't been visited yet
      if (std::find(path.begin(), path.end(), e->getDest()->getInfo()) != path.end()) continue;
      possibleEdges.push(e);
    }
  }
  return possibleEdges;
}

TSPResult btDFS(Graph<Info>& g, const BTPossibility& p, double bestCost) {
  // Base cases
  if (p.possibleEdges.empty()) {
    if (p.path.size() == g.getNumVertex() + 1) {
      if (p.cost < bestCost) bestCost = p.cost;
      return {p.cost, p.path};    // Hamiltonian cycle complete
    } else {
      return {UINT_MAX, p.path};  // Invalid path
    }
  }

  // Bounding
  if (p.cost >= bestCost) return {UINT_MAX, p.path};

  Edge<Info>* e = p.possibleEdges.front();
  std::queue<Edge<Info>*> possibleEdges = p.possibleEdges;
  double cost = p.cost;
  std::vector<Info> path = p.path;
  BTPossibility next;

  // Left choice: add edge to path
  double leftCost = cost + e->getWeight();
  auto leftPath = path;
  leftPath.push_back(e->getDest()->getInfo());
  auto leftPossibleEdges = generatePossibleEdges(g, e->getDest(), leftPath);
  next = {leftCost, leftPossibleEdges, leftPath};
  auto leftResult = btDFS(g, next, bestCost);

  // Right choice: skip edge
  auto rightPossibleEdges = possibleEdges;
  rightPossibleEdges.pop();
  next = {cost, rightPossibleEdges, path};
  auto rightResult = btDFS(g, next, bestCost);

  // Select best choice
  if (leftResult.cost <= rightResult.cost) return leftResult;
  else return rightResult;
}

TSPResult Data::backtracking() {
  Vertex<Info>* start = g.findVertex(Info(START_VERTEX));

  std::queue<Edge<Info>*> possibleEdges = generatePossibleEdges(g, start, {start->getInfo()});
  BTPossibility p = {0, possibleEdges, {start->getInfo()}};
  double bestCost = UINT_MAX;

  return btDFS(g, p, bestCost);
}

// ====================================================================================================

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
