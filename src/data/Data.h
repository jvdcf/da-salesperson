#ifndef DA2324_PRJ1_G163_DATA_H
#define DA2324_PRJ1_G163_DATA_H

#include "../../lib/Graph.h"
#include "../CSV.hpp"
#include "Info.h"
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

/**
 * @brief Data storage and algorithms execution.
 * @details This class is responsible for storing the data and executing the
 * algorithms asked by the Runtime class.
 */

class Data {
private:
  /// Graph with the data inside Info objects.
  Graph<uint64_t> g;

public:
  /**
   * @brief Constructor
   */
  Data(std::string edge_filename);
  Data(std::string edge_filename, std::string node_filename);

  /**
   * @brief Getter for the graph
   */
  Graph<uint64_t> &getGraph();
};

#endif // DA2324_PRJ1_G163_DATA_H
