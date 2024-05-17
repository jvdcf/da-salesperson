#ifndef UTILS
#define UTILS


#include <cstdint>
#include <utility>
#include <chrono>
#include "data/Info.h"
#include "data/Graph.hpp"

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

  static void prim(Graph<Info> *g);

  static std::vector<uint64_t> MSTdfs(Graph<Info> *g);

  static void MSTdfsVisit(Vertex<Info> &v, std::vector<uint64_t> &res, Graph<Info> *g);

  static bool isConnected(Graph<Info> *g);

  static void dfs(Vertex<Info> *v, Graph<Info> * );

  static double weight(uint64_t v, uint64_t u, Graph<Info> *g);


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
