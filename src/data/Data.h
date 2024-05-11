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
};

#endif // DA2324_PRJ1_G163_DATA_H
