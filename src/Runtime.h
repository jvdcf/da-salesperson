#ifndef DA2324_PRJ1_G163_RUNTIME_H
#define DA2324_PRJ1_G163_RUNTIME_H

#include "Parser.h"
#include "Utils.h"
#include "data/Data.h"
#include <array>
#include <cctype>
#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

class CommandLineValue {
public:
  enum Kind {
    Command,
    Ident,
    Int,
    String,
    Sep,
    Code,
  };
  Kind kind;
  CommandLineValue(Kind k, auto v) : kind(k), value(v){};
  std::optional<std::string> getStr();
  std::optional<uint32_t> getInt();
  static Parser<CommandLineValue> parse_ident();

  static Parser<CommandLineValue> parse_int() {
    return verifies(isdigit).take_while().recognize().pmap<CommandLineValue>(
        [](auto inp) {
          return CommandLineValue(Kind::Int, (uint32_t)std::stoul(inp));
        });
  }

  static Parser<CommandLineValue> parse_str() {
    return verifies(isalnum).take_while().recognize().pmap<CommandLineValue>(
        [](auto inp) { return CommandLineValue(Kind::String, inp); });
  }

  static Parser<CommandLineValue> parse_sep(char c) {
    return verifies([c](auto b) { return c == b; })
        .pmap<CommandLineValue>([](auto inp) {
          return CommandLineValue(Kind::Sep, std::to_string(inp));
        });
  }

private:
  std::variant<uint32_t, std::string, std::pair<Info::Kind, uint32_t>> value;
};

class Command {
public:
  enum Cmd {
    Help,
    Quit,
  } command;
  std::vector<CommandLineValue> args;
  Command(Cmd typ, std::vector<CommandLineValue> args)
      : command(typ), args(args){};
};

/**
 * @brief Interface and input handling.
 * @details This class is responsible for reading the user input and calling the
 * appropriate functions.
 */
class Runtime {
private:
  /// Pointer to a Data object. Used to call their methods.
  Data *data;

  /**
   * @brief From a list of arguments, process them and call the appropriate
   * function.
   * @param arg: A vector of strings with the arguments. The first string is the
   * command.
   */
  void processArgs(std::string input);

public:
  /**
   * @brief Constructor
   * @param d: Pointer to a Data object.
   */
  explicit Runtime(Data *d);

  /**
   * @brief Main loop.
   * @details Reads the user input, filters it and calls Runtime::processArgs().
   */
  [[noreturn]] void run();

  static Parser<Command> parse_quit() {
    return ws().pair(string_p("quit")).pair(ws()).pmap<Command>([](auto inp) {
      return Command(Command::Quit, {});
    });
  }

  static Parser<Command> parse_help() {
    return ws().pair(string_p("help")).pair(ws()).pmap<Command>([](auto inp) {
      return Command(Command::Help, {});
    });
  }

  static Parser<Command> parse_cmd() {
    return alt(std::vector({
      parse_help(),
      parse_quit(),
    }));
  }

  void printHelp();
  void handleQuit();
};

#endif // DA2324_PRJ1_G163_RUNTIME_H
