/**
 * @file main.cpp
 * @brief The entry point of the program.
 * @details Checks the validity of the arguments and starts the program.
 */

#include <iostream>
#include <fstream>
#include <filesystem>

#include "src/Utils.h"
#include "src/data/Data.h"
#include "src/CSV.h"
#include "src/Parser.h"
#include "src/Runtime.h"


void printError() {
  // TODO
  std::cerr
      << "USAGE: DA2324_PRJ2_G163 <path>\n"
      << "       being <path> the folder in which the following csv files are located:\n"
      << "        - TODO\n" 
      << "See the Doxygen documentation for more information."
      << std::endl;
  std::exit(1);
}

std::vector<std::string> getCSVPaths(std::string path) {
  // TODO
  std::vector<std::string> expectedFiles = {"TODO"};
  std::vector<std::string> paths = {""};  // {TODOPath, ...}

  for (const auto& file : std::filesystem::directory_iterator(path)) {
    std::string filePath = file.path().string();
    for (int i = 0; i < 4; ++i) {
      if (filePath.find(expectedFiles[i]) != std::string::npos && filePath.find(".csv") != std::string::npos) {
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
  for (const std::string& path: paths) {
    std::ifstream file(path);
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto p = parse_csv()(fileContent);
    if (!p.has_value()) {
      error("Failed to parse the csv file " + path);
      printError();
    }
    auto [rest, v] = p.value();
    csv.push_back(v);
  }
  return csv;
}

int main(int argc, char **argv) {
  panic("main.cpp not implemented yet!");

  if (argc != 2) printError();
  if (!std::filesystem::is_directory(argv[1])) {
    error("The path provided is not a directory (" + std::string(argv[1]) + ")");
    printError();
  }

  // std::vector<std::string> paths = getCSVPaths(argv[1]);
  // std::vector<Csv> csv = parseCSVs(paths);

  // Data d();
  // Runtime rt(&d);
  // rt.run();
}
