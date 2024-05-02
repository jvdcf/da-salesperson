#ifndef CSV_H
#define CSV_H
#define PARSUM_IMPL
#include "Parsum.hpp"
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

/*
 * @brief This class represents the possible values in a CSV file.
 */
class CsvValues {
private:
  /// Type-safe union of possible values.
  std::variant<std::string, int64_t, double, std::nullptr_t> value;

public:
  /// Type that indicates which variant the class holds.
  enum var {
    String,
    Integer,
    Float,
    None,
    Separator,
  } variant;
  /// Constructor
  CsvValues() : value(nullptr), variant(None) {};

  std::optional<std::string> get_str();
  std::optional<int64_t> get_int();
  std::optional<double> get_flt();
  /// Helper function to display the contents of the CsvValue
  std::string display() {
    switch (variant) {
    case String:
      return "Str(" + std::get<std::string>(this->value) + ")";
    case Integer:
      return "Int(" + std::to_string(std::get<int64_t>(this->value)) + ")";
    case Float:
      return "Float(" + std::to_string(std::get<double>(this->value)) + ")";
    case None:
      return "Null";
    case Separator:
      return "Separator";
    }
    return "";
  }
  /// Static constructor for Int
  static CsvValues Int(int64_t i) {
    CsvValues r = CsvValues();
    r.variant = Integer;
    r.value = i;
    return r;
  }
  /// Static constructor for Str
  static CsvValues Str(std::string i) {
    CsvValues r = CsvValues();
    r.variant = String;
    r.value = i;
    return r;
  }
  /// Static constructor for Flt
  static CsvValues Flt(double i) {
    CsvValues r = CsvValues();
    r.variant = Float;
    r.value = i;
    return r;
  }
  /// Static constructor for Nil
  static CsvValues Nil() {
    CsvValues r = CsvValues();
    r.variant = None;
    r.value = nullptr;
    return r;
  }
  /// Static constructor for Sep
  static CsvValues Sep() {
    CsvValues r = CsvValues();
    r.variant = Separator;
    r.value = nullptr;
    return r;
  }
};

/*
 * @brief This class represents a line in a CSV file.
 */
struct CsvLine {
private:
  /// The line
  std::vector<CsvValues> line;
  /// Adds an item to the line
public:
  constexpr CsvLine() : line({}) {};
  constexpr CsvLine(std::vector<CsvValues> line) : line(line) {};
  void add_val(CsvValues v) { line.push_back(v); }
  /// Exposes the underlying data in the line
  const std::vector<CsvValues> &get_data() { return line; }
  /// Displays the contents of the line
  std::string display() {
    std::string res;
    for (auto v : line) {
      res += v.display() + ", ";
    }
    return res;
  }
};

/*
 * @brief Represents a CSV file that does not contain partially filled columns.
 */
class Csv {
private:
  /// The Header containing the title of each column
  CsvLine header;
  /// The actual data
  std::vector<CsvLine> data;

public:
  /// Default Constructor
  constexpr Csv() : header(CsvLine()), data(std::vector<CsvLine>()) {};
  /// Constructor
  constexpr Csv(CsvLine const &head, std::vector<CsvLine> const &dat)
      : header(head), data(dat) {};
  /// Returns the underlying data consuming the CSV.
  std::vector<CsvLine> to_data() { return std::move(data); }
  /// Displays the csv file
  std::string display() {
    std::string res;
    res += "Header: " + header.display() + "\n";
    for (uint64_t i = 0; i < data.size(); ++i) {
      res += "line " + std::to_string(i) + ": " + data[i].display() + "\n";
    }
    return res;
  }
};
/*
 * @brief Parses a CsvLine.
 * @return Parser<CsvLine>
 * @note when called: O(1).
 */
consteval auto parse_line();

/*
 * @brief Parses a Csv file.
 * @return Parser<Csv>
 * @note when called: O(1).
 */
consteval auto parse_csv();

/*
 * @brief Parses a floating point number.
 * @return Parser<CsvValues>
 * @note when called: O(1).
 */
consteval auto parse_flt();

/*
 * @brief Parses a symbol in a Csv.
 * @return Parser<CsvValues>
 * @note when called: O(1).
 */
consteval auto parse_str();

/*
 * @brief Parses an integer.
 * @return Parser<CsvValues>
 * @note when called: O(1).
 */
consteval auto parse_int();

/*
 * @brief Parses the weird notation in one of the given files.
 * @return Parser<CsvValues>
 * @note when called: O(1).
 */
consteval auto parse_weird();

consteval auto parse_sep();

consteval auto p_digits() {
  return parsum::many1(
      parsum::map(parsum::verify([](char const &c) { return isdigit(c); }),
                  [](auto c) { return std::string{c}; }));
}

consteval auto parse_int() {
  constexpr auto parser = parsum::map(
      maybe(parsum::char_p<std::string>('-')) + p_digits(),
      [](auto inp) -> CsvValues { return CsvValues::Int(std::stoll(inp)); });
  return context(parser, [](auto err) {
    return parsum::ParseError("Failed to parse int: " + err.get_why(),
                              err.get_kind(), err.get_pos());
  });
}

consteval auto parse_flt() {
  constexpr auto parser =
      parsum::maybe(parsum::char_p<std::string>('-')) >> p_digits() >>
      parsum::char_p<std::string>('.') >> p_digits() >>
      parsum::cut(
          parsum::peek(parsum::verify([](char const &c) { return c != '.'; })));
  constexpr auto result = map(parser, [](auto inp) -> CsvValues {
    auto [sign, fst, sep, snd] = inp;
    return CsvValues::Flt(std::stod(sign + fst + sep + snd));
  });
  return parsum::context(result, [](auto err) {
    return parsum::ParseError("Failed to parse flt: " + err.get_why(),
                              err.get_kind(), err.get_pos());
  });
}

consteval auto parse_str() {
  constexpr auto fst = parsum::map(parsum::verify([](char const &c) {
                                     return c != ',' && c != '\n' && c != '\r';
                                   }),
                                   [](auto c) { return std::string{c}; });
  constexpr auto parser = parsum::many1(fst);
  constexpr auto result =
      parsum::map(parser, [](auto inp) { return CsvValues::Str(inp); });
  return parsum::context(result, [](auto err) {
    return parsum::ParseError("Failed to parse str: " + err.get_why(),
                              err.get_kind(), err.get_pos());
  });
}

consteval auto parse_weird() {
  constexpr auto parse_quot = parsum::char_p('"');
  constexpr auto parse_comm = parsum::char_p(',');
  constexpr auto parser =
      parse_quot >> p_digits() >> parse_comm >> p_digits() >> parse_quot;
  constexpr auto result = parsum::map(
      parser, [](std::tuple<char, std::string, char, std::string, char> t) {
        auto [_1, p1, _2, p2, _3] = t;
        return CsvValues::Int(std::stoll(p1 + p2));
      });
  return parsum::context(result, [](auto err) {
    return parsum::ParseError("Failed to parse weird: " + err.get_why(),
                              err.get_kind(), err.get_pos());
  });
}
consteval auto parse_sep() {
  constexpr auto result =
      parsum::map(parsum::char_p(','), [](auto c) { return std::tuple(); });
  return parsum::context(result, [](auto err) {
    return parsum::ParseError("Failed to parse flt: " + err.get_why(),
                              err.get_kind(), err.get_pos());
  });
}

consteval auto parse_line() {
  using parsum::char_p;
  constexpr auto parse_bom = parsum::context(
      parsum::map(char_p('\xEF') >> char_p('\xBB') >> char_p('\xBF'),
                  [](auto) { return std::tuple(); }),
      [](auto err) {
        return parsum::ParseError("BOM: " + err.get_why(), err.get_kind(),
                                  err.get_pos());
      });
  constexpr auto parse_val = parsum::context(
      parse_flt() | parse_int() | parse_weird() | parse_str(), [](auto err) {
        return parsum::ParseError("No valid alternatives found!",
                                  err.get_kind(), err.get_pos());
      });
  constexpr auto parse_endl = parsum::context(
      parsum::map(many0(char_p<std::string>('\r') | char_p<std::string>('\n')),
                  [](std::string const &) { return std::tuple(); }),
      [](auto err) {
        return parsum::ParseError("Newline: " + err.get_why(), err.get_kind(),
                                  err.get_pos());
      });
  constexpr auto parser =
      parsum::maybe(parse_bom) >>
      many1(parsum::map(parse_val >> maybe(parse_sep()),
                        [](auto c) { return std::vector{std::get<0>(c)}; })) >>
      parse_endl;
  constexpr auto result = parsum::map(parser, [](auto c) {
    auto [pp] = c;
    CsvLine res(pp);
    return res;
  });
  return parsum::context(result, [](auto err) {
    return parsum::ParseError("Failed to parse line: " + err.get_why(),
                              err.get_kind(), err.get_pos());
  });
}

consteval auto parse_csv() {
  constexpr auto parser =
      parse_line() >>
      parsum::many0(parsum::map(
          parse_line(), [](CsvLine const &c) { return std::vector{c}; }));
  constexpr auto result = parsum::map(parser, [](auto c) {
    auto [head, data] = c;
    return Csv(head, data);
  });
  return parsum::context(result, [](auto err) {
    return parsum::ParseError("Could not parse CSV: " + err.get_why(),
                              err.get_kind(), err.get_pos());
  });
}

inline std::optional<std::string> CsvValues::get_str() {
  if (variant != String) {
    return {};
  } else {
    return std::get<std::string>(this->value);
  }
}

inline std::optional<int64_t> CsvValues::get_int() {
  if (variant != Integer) {
    return {};
  } else {
    return std::get<int64_t>(this->value);
  }
}

inline std::optional<double> CsvValues::get_flt() {
  if (variant != Float) {
    return {};
  } else {
    return std::get<double>(this->value);
  }
}
#endif // CSV_H
