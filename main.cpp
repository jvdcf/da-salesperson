/**
 * @file main.cpp
 * @brief The entry point of the program.
 * @details Checks the validity of the arguments and starts the program.
 */

#include <filesystem>
#include <fstream>
#include <iostream>

#include "src/CSV.hpp"
#include "src/Parsum.hpp"
#include "src/Runtime.h"
#include "src/Utils.h"
#include "src/data/Data.h"

void printError() {
  std::cerr << "USAGE: DA2324_PRJ2_G163 <edges.csv> [<nodes.csv>] \n"
            << "       being <edges.csv> the path to the csv file containing the edges\n"
            << "       and [<nodes.csv>] an optional path to the csv files about the nodes.\n"
            << "See the Doxygen documentation for more information.\n";
  std::exit(1);
}

bool isFile(const std::string &path) {
  if (!std::filesystem::is_regular_file(path)) {
    error("The path provided is not a file (" + path + ")");
    return false;
  } else if (path.substr(path.find_last_of('.') + 1) != "csv") {
    error("The file provided is not a csv file (" + path + ")");
    return false;
  }
  return true;
}

void startProgram(Data &d, Clock &c) {
  Runtime rt(&d);
  c.stop();
  std::ostringstream oss;
  oss << "Parsing took " << c;
  info(oss.str());
  rt.run();
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 3) {
    for (int i = 0; i < argc; i++) {
      std::cout << argv[i] << std::endl;
    }
    printError();
  }
  if (!isFile(argv[1])) printError();

  Clock c; c.start();
  if (argc == 2 || std::string(argv[2]).empty()) {
    Data d(argv[1]);
    startProgram(d, c);
  } else {
    if (!isFile(argv[2])) printError();
    Data d(argv[1], argv[2]);
    startProgram(d, c);
  }
}
