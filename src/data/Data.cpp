#include "Data.h"
#include "../CSV.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

// Constructor

Data::Data(std::string edge_filename) {
  constexpr auto parser = parse_line().to_fn();
  this->g = Graph<uint64_t>();
  std::ifstream file(edge_filename, std::ios::in | std::ios::binary);
  const auto size = std::filesystem::file_size(edge_filename);
  std::string file_contents(size, '\0');
  file.read(file_contents.data(), size);
  std::istringstream input(file_contents);
  auto header = parser(input);
  auto res = parsum::Result<CsvLine, parsum::ParseError>(CsvLine());
  while ((res = parser(input)).has_val) {
    auto line = res.ok.get_data();
    auto orig = line[0].get_int();
    auto dest = line[1].get_int();
    auto dist = line[2].get_flt();
    if (orig.has_value() && dest.has_value() && dist.has_value()) {
      auto o = this->g.findVertex(orig.value());
      if (o == nullptr) {
        // TODO: fix this to be faster!
        this->g.addVertex(orig.value());
        o = this->g.findVertex(orig.value());
      }
      auto d = this->g.findVertex(dest.value());
      if (d == nullptr) {
        // TODO: fix this to be faster!
        this->g.addVertex(dest.value());
        d = this->g.findVertex(dest.value());
      }
      this->g.addEdge(o, d, dist.value());
    }
  }
}

Graph<uint64_t> &Data::getGraph() { return g; }

// Functions
// =================================================================================================
