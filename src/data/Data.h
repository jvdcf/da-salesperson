#ifndef DA2324_PRJ1_G163_DATA_H
#define DA2324_PRJ1_G163_DATA_H

#include "../CSV.hpp"
#include "Graph.hpp"
#include "Info.h"
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

typedef bool (*savefn_t)(std::vector<CsvValues> const &, Graph<Info> &);

#define START_VERTEX 0

/**
 * @brief Result of the Travelling Salesman Problem
 */
struct TSPResult {
  /// Cost of the best path
  double cost;
  /// Path that starts and ends at the same vertex (size = number of vertices + 1)
  std::vector<uint64_t> path;

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
  /**
   * @brief Constructor with coordinates
   */
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
   * @note Time Complexity: O(V!) where V is the number of vertices
   * @return A TSPResult with the cost of the best path and the path itself
   */
  TSPResult backtracking();

  /**
   * @brief 2-approximation algorithm to approximate the Travelling Salesman Problem
   * @details This algorithm generates a Minimum Spanning Tree and then traverses it in a Depth-First Search.
   * The path is corrected afterwards to make it a valid TSP path.
   * @note Time Complexity: O((V + E) log V) where V is the number of vertices and E is the number of edges
   * @return A TSPResult with the cost of the best path and the path itself
   */
  TSPResult triangular();

  /**
   * @brief Nearest Neighbor algorithm to approximate the Travelling Salesman Problem
   * @details Starting at 0, the algorithm chooses the lightest edge to the next vertex until all vertices are visited.
   * @note Time Complexity: O(V^2) where V is the number of vertices
   * @return A TSPResult with the cost of the best path and the path itself
   */
  TSPResult heuristic();

  /**
   * @brief Ant Colony Optimization algorithm to approximate the Travelling Salesman Problem
   * @details Using statistical methods, the algorithm simulates the behavior of ants to find the best path.
   * When selecting the next vertex, the algorithm uses a probability distribution based on the pheromones and the distance.
   * For each iteration, the pheromones are updated based on the best path found (if it exists).
   * @note Time Complexity: O(V * E) where V is the number of vertices and E is the number of edges
   * @param vertexId The vertex to start the algorithm
   * @param iterations The number of iterations to run the algorithm
   * @return A TSPResult, if a path was found, or an empty optional if otherwise
   */
  std::optional<TSPResult> disconnected(uint64_t vertexId, unsigned iterations);
};

#endif // DA2324_PRJ1_G163_DATA_H
