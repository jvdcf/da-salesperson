#include "Runtime.h"
#include "Parsum.hpp"
#include "Utils.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

Runtime::Runtime(Data *d) { this->data = d; }

[[noreturn]] void Runtime::run() {
  std::cout << "Welcome to Water Supply Management.\n"
            << "Type 'help' to learn more.\n";

  std::string input, buf;

  while (true) {
    input.clear();

    std::cout << "> ";
    std::getline(std::cin, input);
    std::istringstream iss(input);
    processArgs(iss);
  }
}

void Runtime::printHelp() {
  auto keyword = Color(0, 255, 0).foreground();
  auto comment = Color(255, 255, 15).foreground();
  std::cout << "Available commands:\n"
            << keyword << "  quit\n"
            << comment << "      Quits this program.\n"
            << keyword << "  help\n"
            << comment << "      Prints this help.\n"
            << Color::clear() << std::endl;
}

void Runtime::handleQuit() {
  info("Quitting...");
  exit(0);
}

void Runtime::processArgs(std::istream &args) {
  constexpr auto cmd_parser = parse_cmd();
  auto cmd_res = parse_cmd().to_fn()(args);
  if (!cmd_res.has_val) {
    std::string arg_contents(std::istreambuf_iterator<char>(args), {});
    return error("The command '" + arg_contents +
                 "' is invalid. Type 'help' to know more.");
  }
  auto cmd = cmd_res.ok;

  std::string rest(std::istreambuf_iterator<char>(args), {});
  if (!rest.empty())
    warning("Trailing output: '" + rest + "'.");
  switch (cmd.command) {
  case Command::Help:
    return printHelp();
  case Command::Quit:
    return handleQuit();
  default:
    error("AAAAAAAAAAAAAAAAAAAAAAA");
    break;
  }

  info("Type 'help' to see the available commands.");
}
