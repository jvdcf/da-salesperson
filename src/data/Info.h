#ifndef DA2324_PRJ1_G163_INFO_H
#define DA2324_PRJ1_G163_INFO_H


#include <cstdint>
#include <string>
#include <variant>
#include <optional>

/**
 * @brief Information inside a Vertex.
 * @details This class is used to store information inside a Vertex.\n
 */

class Info {
public:

  /**
   * @brief Enum with the possible kinds of Vertex.
   */
  enum Kind {
    // TODO
  };

  /**
   * @brief Constructor
   */
  Info();

  /**
   * @brief Getter for the kind of Vertex
   * @return Info::Kind
   */
  Kind getKind() const;

  /**
   * @brief Getter for the id number
   * @return Id number
   */
  uint16_t getId() const;

  /// Get active status
  bool isActive() const;
  /// Disable (is_active = false)
  void disable();
  /// Enable (is_active = true)
  void enable();

  /**
   * @brief Equality operator
   */
  bool operator==(const Info &rhs) const;

private:
  // TODO
  /// Is active boolean
  bool is_active;
};

#endif //DA2324_PRJ1_G163_INFO_H
