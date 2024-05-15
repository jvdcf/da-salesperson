#ifndef DA2324_PRJ1_G163_INFO_H
#define DA2324_PRJ1_G163_INFO_H


#include <cstdint>
#include <string>
#include <variant>
#include <optional>

/**
 * @brief Information inside a Vertex.
 * @details This class is used to store the id number and the coordinates of a Vertex.
 */

class Info {
public:
  Info() = default;

  /**
   * @brief Constructor (using edges.csv)
   */
  explicit Info(uint64_t id);

  /**
   * @brief Constructor (using nodes.csv)
   */
  Info(uint64_t id, double lat, double lon);

  /**
   * @brief Getter for the id number
   * @return Id number
   */
  [[nodiscard]] uint64_t getId() const;

  /**
   * @brief Getter for the latitude
   * @return Latitude
   */
  [[nodiscard]] std::optional<double> getLat() const;

  /**
   * @brief Getter for the longitude
   * @return Longitude
   */
  [[nodiscard]] std::optional<double> getLon() const;

  /**
   * @brief Distance between two Vertexes
   * @param other The other Vertex
   * @return Distance between the two Vertexes
   */
  [[nodiscard]] double distance(const Info &other) const;

  /**
   * @brief Equality operator
   */
  bool operator==(const Info &rhs) const;

private:
  /// Id number of the Vertex
  uint64_t id;
  /// Latitude (if applicable)
  std::optional<double> lat;
  /// Longitude (if applicable)
  std::optional<double> lon;
};

#endif //DA2324_PRJ1_G163_INFO_H
