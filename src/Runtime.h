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
  std::optional<std::string> getStr() {
    if (this->kind == String)
      return std::get<std::string>(this->value);
    else
      return {};
  };
  std::optional<uint32_t> getInt() {
    if (this->kind == Int)
      return std::get<uint32_t>(this->value);
    else
      return {};
  };

  // static Parser<CommandLineValue> parse_ident();

  static consteval auto parse_int() {
    return parsum::map(parsum::digits1(), [](std::string num) {
      return CommandLineValue(Kind::Int, (uint32_t)std::stoul(num));
    });
  }

  static consteval auto parse_str() {
    return parsum::map(parsum::alphanumerics1(), [](auto s) {
      if (s.find_first_not_of("0123456789") != std::string::npos) {
        return CommandLineValue(Kind::String, s);
      } else {
        throw std::exception();
      }
    });
  }

  static consteval auto parse_sep(char const &c) {
    return parsum::map(parsum::char_p<std::string>(c),
                       [](auto s) { return CommandLineValue(Kind::Sep, s); });
  }

private:
  std::variant<std::string, uint32_t> value;
};

class Command {
public:
  enum Cmd {
    Help,
    Quit,
    Count,
    Backtracking,
    Triangular,
    Heuristic,
    Disconnected,
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
  /// Clock object to measure the time of the algorithms.
  Clock clock;

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
    using parsum::string_p;
    return parsum::map(parsum::ws0() >> string_p("quit") >> parsum::ws0(),
                       [](auto c) { return Command(Command::Quit, {}); });
  }

  static consteval auto parse_help() {
    using parsum::string_p;
    return parsum::map(parsum::ws0() >> string_p("help") >> parsum::ws0(),
                       [](auto c) { return Command(Command::Help, {}); });
  }

  static consteval auto parse_count() {
    using parsum::string_p;
    return parsum::map(parsum::ws0() >> string_p("count") >> parsum::ws0(),
                       [](auto c) { return Command(Command::Count, {}); });
  }

  static consteval auto parse_backtracking() {
    using parsum::string_p;
    return parsum::map(parsum::ws0() >> string_p("backtracking") >> parsum::ws0(),
                       [](auto c) { return Command(Command::Backtracking, {}); });
  }

  static consteval auto parse_triangular() {
    using parsum::string_p;
    return parsum::map(parsum::ws0() >> string_p("triangular") >> parsum::ws0(),
                       [](auto c) { return Command(Command::Triangular, {}); });
  }

  static consteval auto parse_heuristic() {
    using parsum::string_p;
    return parsum::map(parsum::ws0() >> string_p("heuristic") >> parsum::ws0(),
                       [](auto c) { return Command(Command::Heuristic, {}); });
  }

  static consteval auto parse_disconnected() {
    using parsum::string_p;
    return parsum::map(
            parsum::ws0() >> string_p("disconnected") >> parsum::ws1() >>
            CommandLineValue::parse_int() >> parsum::ws1() >> CommandLineValue::parse_int() >> parsum::ws0(),
            [](auto inp) {
              auto [a, b, c, val, d, iter, e] = inp;
              return Command(Command::Disconnected, {val, iter}); });
  }

  static consteval auto parse_cmd() {
    return parse_quit() | parse_help()
         | parse_count() | parse_backtracking() | parse_triangular() | parse_heuristic() | parse_disconnected();
  }

  void printHelp();
  void handleQuit();
  void handleCount();
  void handleBacktracking();
  void handleTriangular();
  void handleHeuristic();
  void handleDisconnected(Command &cmd);
};

#endif // DA2324_PRJ1_G163_RUNTIME_H
