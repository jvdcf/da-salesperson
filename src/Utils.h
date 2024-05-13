#ifndef UTILS
#define UTILS


#include <cstdint>
#include <utility>
#include <chrono>
#include "data/Info.h"
#include "../lib/Graph.h"


/**
 * @brief Auxiliary functions
 * @details This class contains static and auxiliary functions used throughout the project.
 */

class Utils {
public:
  static unsigned countLines(const std::string& path);
  static void printLoading(unsigned current, unsigned total, const std::string& path);
  static void clearLine();

  static double convertToRadians(double angle);
  static double haversineDistance(double lat1, double lon1, double lat2, double lon2);

  static std::vector<Vertex<Info> *> prim(Graph<Info> *g);

  static std::vector<Vertex<Info> *> MSTdfs(const std::vector<Vertex<Info> *>& vertexSet);

  static void dfsVisit(Vertex<Info> *v, std::vector<Vertex<Info> *> &res);
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

class Clock {
public:
  Clock();
  void start();
  void stop();
  [[nodiscard]] double getTime() const;
  friend std::ostream& operator<<(std::ostream& os, const Clock& c) {
    os << c.getTime() << "ms";
    return os;
  }
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
};



#endif // !UTILS
