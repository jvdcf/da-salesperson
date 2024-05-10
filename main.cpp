/**
 * @file main.cpp
 * @brief The entry point of the program.
 * @details Checks the validity of the arguments and starts the program.
 */

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "src/CSV.hpp"
#include "src/Parsum.hpp"
#include "src/Runtime.h"
#include "src/Utils.h"
#include "src/data/Data.h"

void printError() {
  // TODO
  std::cerr << "USAGE: DA2324_PRJ2_G163 <path>\n"
            << "       being <path> the folder in which the following csv "
               "files are located:\n"
            << "        - TODO\n"
            << "See the Doxygen documentation for more information."
            << std::endl;
  std::exit(1);
}

std::vector<std::string> getCSVPaths(std::string path) {
  // TODO
  std::vector<std::string> expectedFiles = {"TODO"};
  std::vector<std::string> paths = {""}; // {TODOPath, ...}

  for (const auto &file : std::filesystem::directory_iterator(path)) {
    std::string filePath = file.path().string();
    for (int i = 0; i < 4; ++i) {
      if (filePath.find(expectedFiles[i]) != std::string::npos &&
          filePath.find(".csv") != std::string::npos) {
        if (paths[i] != "") {
          error("Found multiple " + expectedFiles[i] + " files");
          printError();
        }
        paths[i] = filePath;
      }
    }
  }

  for (int i = 0; i < 4; i++) {
    if (paths[i].empty()) {
      error("Missing " + expectedFiles[i] + " file");
      printError();
    }
  }
  return paths;
}

std::vector<Csv> parseCSVs(std::vector<std::string> paths) {
  std::vector<Csv> csv;
  for (const std::string &path : paths) {
    std::ifstream file(path);
    std::string fileContent((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    std::istringstream contents(fileContent);
    constexpr auto parser = parse_csv();
    auto p = parsum::parse_fn(parser)(contents);
    if (!p.has_val) {
      error("Failed to parse the csv file " + path);
      printError();
    }
    csv.push_back(p.ok);
  }
  return csv;
}

int main(int argc, char **argv) {
  if (argc != 2)
    return 1;
  Data d(argv[0]);
  Runtime rt(&d);
  rt.run();
  panic("main.cpp not implemented yet!");

  if (argc != 2)
    printError();
  if (!std::filesystem::is_directory(argv[1])) {
    error("The path provided is not a directory (" + std::string(argv[1]) +
          ")");
    printError();
  }

  // std::vector<std::string> paths = getCSVPaths(argv[1]);
  // std::vector<Csv> csv = parseCSVs(paths);

  // Data d();
  // Runtime rt(&d);
  // rt.run();
}
