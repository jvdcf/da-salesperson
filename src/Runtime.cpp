#include "Runtime.h"
#include "Parser.h"
#include "Utils.h"
#include <cstdint>
#include <exception>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

Runtime::Runtime(Data *d) { this->data = d; }

[[noreturn]] void Runtime::run() {
  std::cout << "Welcome to Water Supply Management.\n"
            << "Type 'help' to learn more.\n";

  std::vector<std::string> args;
  std::string input, buf;
  std::istringstream iss;

  while (true) {
    args.clear();
    input.clear();
    buf.clear();

    std::cout << "> ";
    getline(std::cin, input);
    if (input.empty())
      continue;
    // processArgs(input);
  }
}

void Runtime::printHelp() {
  auto keyword = Color(0,255,0).foreground();
  auto comment = Color(255,255,15).foreground();
  std::cout
      << "Available commands:\n"
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

void Runtime::processArgs(std::string args) {
  POption<Command> cmd_res = parse_cmd()(args);
  if (!cmd_res.has_value())
    return error("The command '" + args +
                 "' is invalid. Type 'help' to know more.");
  auto [rest, cmd] = cmd_res.value();
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
