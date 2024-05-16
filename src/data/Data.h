#ifndef DA2324_PRJ1_G163_DATA_H
#define DA2324_PRJ1_G163_DATA_H

#include "Graph.hpp"
#include "../CSV.hpp"
#include "Info.h"
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

typedef bool (*savefn_t)(std::vector<CsvValues> const &, Graph<Info> &);

#define START_VERTEX 0

struct TSPResult {
  std::vector<uint64_t> path;
  double cost;

  friend std::ostream &operator<<(std::ostream &os, const TSPResult &res) {
    os << "Cost: " << res.cost << " | ";
    os << "Path: ";
    for (const auto &i : res.path) {
      os << i << " ";
    }
    return os;
  }

  bool operator<(const TSPResult &res) const {
    return this->cost < res.cost;
  }
};

/**
 * @brief Data storage and algorithms execution.
 * @details This class is responsible for storing the data and executing the
 * algorithms asked by the Runtime class.
 */

class Data {
private:
  /// Graph with the data inside Info objects.
  Graph<Info> g;

  std::istringstream prepareCsv(const std::string &path);
  bool static saveEdge(std::vector<CsvValues> const &line, Graph<Info> &g);
  bool static saveNode(std::vector<CsvValues> const &line, Graph<Info> &g);
  void parseCsv(const std::string &path, Graph<Info> &g, const savefn_t saveFn);

public:
  /**
   * @brief Constructor
   */
  explicit Data(const std::string &edge_filename);
  Data(const std::string &edge_filename, const std::string &node_filename);

  /**
   * @brief Getter for the graph
   */
  Graph<Info> &getGraph();

  /**
   * @brief Backtracking algorithm to solve the Travelling Salesman Problem
   * @details Bounding:
   * - If the current cost is already higher than the best cost, stop exploring this path
   * - If the current path reaches a vertex that has already been visited, stop exploring this path
   * @note Time Complexity: O(n!) where n is the number of vertices
   * @return A TSPResult with the cost of the best path and the path itself
   */
  TSPResult backtracking();
  TSPResult triangular();
  TSPResult heuristic();
  std::optional<TSPResult> disconnected(uint64_t vertexId);
};

#endif // DA2324_PRJ1_G163_DATA_H
