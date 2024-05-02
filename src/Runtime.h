#ifndef DA2324_PRJ1_G163_RUNTIME_H
#define DA2324_PRJ1_G163_RUNTIME_H

#include "Parsum.hpp"
#include "Utils.h"
#include "data/Data.h"
#include <array>
#include <cctype>
#include <cstdint>
#include <exception>
#include <istream>
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
  CommandLineValue(Kind k, auto v) : kind(k), value(v) {};
  std::optional<std::string> getStr();
  std::optional<uint32_t> getInt();
  // static Parser<CommandLineValue> parse_ident();

  static consteval auto parse_int() {
    return parsum::map(parsum::many1(parsum::digit()), [](std::string num) {
      return CommandLineValue(Kind::Int, (uint32_t)std::stoul(num));
    });
  }

  static consteval auto parse_str() {
    return parsum::map(
        parsum::many1(parsum::map(
            parsum::verify([](char const &c) { return isalnum(c); }),
            [](char const &c) { return std::string{c}; })),
        [](auto s) { return CommandLineValue(Kind::String, s); });
  }

  static consteval auto parse_sep(char const &c) {
    return parsum::map(parsum::char_p<std::string>(c),
                       [](auto s) { return CommandLineValue(Kind::Sep, s); });
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
      : command(typ), args(args) {};
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
  void processArgs(std::istream &input);

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

  static consteval auto parse_quit() {
    // return ws().pair(string_p("quit")).pair(ws()).pmap<Command>([](auto inp)
    // {
    //   return Command(Command::Quit, {});
    // });
    using parsum::char_p;
    constexpr auto quit =
        char_p('q') >> char_p('u') >> char_p('i') >> char_p('t');
    return parsum::map(parsum::ws0() >> quit >> parsum::ws0(),
                       [](auto c) { return Command(Command::Quit, {}); });
  }

  static consteval auto parse_help() {
    using parsum::char_p;
    constexpr auto help =
        char_p('h') >> char_p('e') >> char_p('l') >> char_p('p');
    return parsum::map(parsum::ws0() >> help >> parsum::ws0(),
                       [](auto c) { return Command(Command::Help, {}); });
  }

  static consteval auto parse_cmd() { return parse_quit() | parse_help(); }

  void printHelp();
  void handleQuit();
};

#endif // DA2324_PRJ1_G163_RUNTIME_H
