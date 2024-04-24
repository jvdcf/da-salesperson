#ifndef UTILS
#define UTILS


#include <cstdint>
#include <utility>
#include "data/Info.h"
#include "../lib/Graph.h"


/**
 * @brief Auxiliary functions
 * @details This class contains static and auxiliary functions used throughout the project.
 */

class Utils {
public:
  // TODO
};


class Color {
public:
  Color(uint8_t r, uint8_t g, uint8_t b): red(r),green(g),blue(b){};
  std::string foreground() const {
#ifndef _WIN32
    return "\033[38;2;" + std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue) + "m";
#else
    return "";
#endif
  }

  std::string background() const {
#ifndef _WIN32
    return "\033[48;2;" + std::to_string(red) + ";" + std::to_string(green) + ";" + std::to_string(blue) + "m";
#else
    return "";
#endif
  }
  static std::string clear() {
#ifndef _WIN32
    return "\033[0m";
#else
    return "";
#endif
  }

private:
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

[[noreturn]] void panic(std::string s);

void error(std::string s);
void info(std::string s);
void warning(std::string s);
#endif // !UTILS
