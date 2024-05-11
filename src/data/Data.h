#ifndef DA2324_PRJ1_G163_DATA_H
#define DA2324_PRJ1_G163_DATA_H

#include "../../lib/Graph.h"
#include "../CSV.hpp"
#include "Info.h"
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <functional>

struct TSPResult {
  std::vector<Info> path;
  double cost;

  friend std::ostream& operator<<(std::ostream& os, const TSPResult& res) {
    os << "Cost: " << res.cost << std::endl;
    os << "Path: ";
    for (const auto& i : res.path) {
      os << i.getId() << " ";
    }
    return os;
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

  std::istringstream prepareCsv(const std::string& path);
  bool static saveEdge(std::vector<CsvValues> line, Graph<Info>& g);
  bool static saveNode(std::vector<CsvValues> line, Graph<Info>& g);
  void parseCsv(const std::string &path, Graph<Info> &g,
                const std::function<bool(std::vector<CsvValues>, Graph<Info> &)> &saveFn);

public:
  /**
   * @brief Constructor
   */
  explicit Data(const std::string& edge_filename);
  Data(const std::string& edge_filename, const std::string& node_filename);

  /**
   * @brief Getter for the graph
   */
  Graph<Info> &getGraph();

  TSPResult backtracking();
  TSPResult triangular();
  TSPResult heuristic();
  std::optional<TSPResult> disconnected(uint64_t vertexId);
};

#endif // DA2324_PRJ1_G163_DATA_H
