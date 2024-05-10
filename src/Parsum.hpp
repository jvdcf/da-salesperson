#ifndef LIB_PARSUM_HPP
#define LIB_PARSUM_HPP

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <istream>
#include <iterator>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

/**
 * @brief Inspired by
 * https://github.com/Qqwy/cpp-parser_combinators/blob/main/optimized/main.cc.
 * */
namespace parsum {
// join_tup()

/** Joins two types into a tuple of them */
template <typename T, typename U>
std::tuple<T, U> constexpr join_tup(T const &lhs, U const &rhs) {
  return std::make_tuple(lhs, rhs);
}
/// Joins a tuple and a type into a tuple with the type at the last place
template <typename... Ts, typename U>
std::tuple<Ts..., U> constexpr join_tup(std::tuple<Ts...> const &lhs,
                                        U const &rhs) {
  return std::tuple_cat(lhs, std::make_tuple(rhs));
}
/// Joins a type and a tuple into a tuple with the type at the first place
template <typename T, typename... Us>
std::tuple<T, Us...> constexpr join_tup(T const &lhs,
                                        std::tuple<Us...> const &rhs) {
  return std::tuple_cat(std::make_tuple(lhs), rhs);
}

/// Joins two tuples into a tuple
template <typename... Ts, typename... Us>
std::tuple<Ts..., Us...> constexpr join_tup(std::tuple<Ts...> const &lhs,
                                            std::tuple<Us...> const &rhs) {
  return std::tuple_cat(lhs, rhs);
}
// join_tup() END

/// Converts a tuple of types into an object that also contains those types in
/// that order
template <typename CT, typename... Ts>
constexpr CT from_tup(std::tuple<Ts...> const &tup) {
  return std::apply([](auto... elems) { return CT{elems...}; }, tup);
}

/// Adds/concats all the values into the Out type.
template <typename Out, typename... Ts>
Out concat_all(Out default_val, Ts... elems) {
  return (default_val + ... + elems);
}
/// Specialized implementation of from_tup with CT = std:string
template <typename... Ts> std::string from_tup(std::tuple<Ts...> const &tup) {
  return std::apply(
      [](auto... elems) { return concat_all(std::string(), elems...); }, tup);
}
// From tup end

/// wrapper for from_tup
template <typename In, typename Out> Out into(In const &val) {
  return from_tup<Out>(val);
}
/// specialization
template <> inline std::string into(std::vector<char> const &val) {
  return std::string(val.begin(), val.end());
}

/// specialization
template <> inline std::vector<char> into(std::string const &val) {
  return std::vector<char>(val.begin(), val.end());
}

/// specialization
template <> inline std::string into(char const &val) { return {val}; }

/// specialization
template <typename T> std::vector<T> into(T const &val) { return {val}; }

/// specialization
template <> inline std::string into(std::tuple<char, std::string> const &val) {
  auto [ch, st] = val;
  return std::string{ch} + st;
}

/// specialization
template <> inline std::string into(std::tuple<std::string, char> const &val) {
  auto [st, ch] = val;
  return st + std::string{ch};
}

/// specialization
template <> inline std::string into(std::tuple<char, char> const &val) {
  auto [ch1, ch2] = val;
  return std::string{ch1, ch2};
}

/// specialization
template <>
inline std::string into(std::tuple<char, char, std::string> const &val) {
  auto [ch1, ch2, st] = val;
  return std::string{ch1, ch2} + st;
}
// INTO end

// ParseError
/// Constexpr struct that contains an error message, level of error, and
/// position.
struct ParseError {
public:
  /// Level of error.
  enum class ErrorVariant {
    Recoverable,
    Irrecoverable,
  };

  /// Constructor
  constexpr ParseError(std::string const &why, ErrorVariant kind, uint64_t pos)
      : why(why), pos(pos), kind(kind) {};
  /// Getter for the level of error
  constexpr ErrorVariant get_kind() { return this->kind; }
  /// Getter for message
  constexpr std::string const &get_why() const { return this->why; }
  /// Getter for stream position
  constexpr uint64_t get_pos() const { return pos; }
  /// Calculates the coordinates of the error, taking the original collection in
  /// full
  template <typename T>
  constexpr std::tuple<uint64_t, uint64_t>
  get_coord(T const &collection) const {
    uint64_t col = 0;
    uint64_t line = 0;
    uint64_t c_pos = 0;
    for (auto c : collection) {
      if (this->get_pos() == c_pos) {
        return {col + 1, line + 1};
      }
      c_pos++;
      if (c == '\n') {
        line++;
        col = 0;
        continue;
      } else {
        col++;
      }
    }
    return {col + 1, line + 1};
  }
  /// Returns a formatted string with the error information
  template <typename T>
  constexpr std::string display(T const &collection) const {
    auto [column, line] = get_coord(collection);
    return "Found error at line " + std::to_string(line) + ", column " +
           std::to_string(column) + ": " + std::string(this->why);
  }
  /// Assign operator
  constexpr ParseError &operator=(ParseError const &other) {
    this->why = other.why;
    this->pos = other.pos;
    this->kind = other.kind;
    return *this;
  }

private:
  std::string why;
  uint64_t pos;
  ErrorVariant kind;
};
// ParseError end

/**
 *
 *
 */
template <typename T, typename E> struct Result {
  bool has_val = false;
  union {
    T ok;
    E err;
  };
  constexpr Result(T const &val) : has_val(true), ok(val) {};
  constexpr Result(E const &err) : has_val(false), err(err) {};
  constexpr Result(Result<T, E> const &other) {
    this->has_val = other.has_val;
    if (other.has_val)
      this->ok = other.ok;
    else
      this->err = other.err;
  }
  constexpr Result<T, E> &operator=(Result<T, E> const &other) {
    this->has_val = other.has_val;
    if (other.has_val)
      this->ok = other.ok;
    else
      this->err = other.err;
    return *this;
  }
  ~Result() {
    if (has_val) {
      ok.~T();
    } else
      err.~E();
  }
  template <typename Tgt> operator Result<Tgt, E>() const {
    if (this->has_val) {
      return into<T, Tgt>(this->ok);
    } else
      return this->err;
  }
};

template <typename P, typename F>
consteval auto map(P const &parser, F const &fn);

template <typename P, typename F, typename Res> struct Map;

template <typename Derived, typename Out> struct Parser {
  typedef Out value_type;
  constexpr auto to_fn() const {
    return static_cast<Derived const &>(*this).to_fn_impl();
  }
  template <typename Out2> operator Parser<Derived, Out2>() const {
    return map(*this, [](Out const &val) { return convert(val); });
  }
};

template <typename Fn> struct VerifyP : public Parser<VerifyP<Fn>, char> {
  Fn fn;
  using Parser<VerifyP<Fn>, char>::Parser;
  constexpr VerifyP(Fn const &fn) : fn(fn) {};
  constexpr auto to_fn_impl() const {
    auto fn_ = this->fn;
    return [fn_](std::istream &inp) -> Result<char, ParseError> {
      if (!inp) {
        inp.clear();
        return ParseError("Reached end of file!",
                          ParseError::ErrorVariant::Recoverable, inp.tellg());
      }
      const char val = inp.peek();
      if (fn_(val)) {
        inp.ignore();
        return val;
      } else {
        return ParseError("Char did not match!",
                          ParseError::ErrorVariant::Recoverable, inp.tellg());
      }
    };
  }
};

template <typename P1, typename P2,
          typename Out =
              decltype(join_tup(std::declval<typename P1::value_type>(),
                                std::declval<typename P2::value_type>()))>
struct Sequence : public Parser<Sequence<P1, P2>, Out> {
  P1 p1;
  P2 p2;
  using Parser<Sequence<P1, P2>, Out>::Parser;
  constexpr Sequence(P1 const &p1, P2 const &p2) : p1(p1), p2(p2) {};

  constexpr auto to_fn_impl() const {
    auto p1_impl = p1.to_fn();
    auto p2_impl = p2.to_fn();
    return [p1_impl, p2_impl](std::istream &inp) -> Result<Out, ParseError> {
      auto pos = inp.tellg();
      auto res1 = p1_impl(inp);
      if (!res1.has_val) {
        inp.seekg(pos);
        inp.clear();
        return res1.err;
      }
      auto res2 = p2_impl(inp);
      if (!res2.has_val) {
        inp.seekg(pos);
        inp.clear();
        return res2.err;
      }
      return join_tup(res1.ok, res2.ok);
    };
  }
};

template <typename P1, typename P2, typename Out = typename P2::value_type>
struct Alternative : public Parser<Alternative<P1, P2>, Out> {
  using Parser<Alternative<P1, P2>, Out>::Parser;

  P1 p1;
  P2 p2;
  constexpr Alternative(P1 const &p1, P2 const &p2) : p1(p1), p2(p2) {};

  constexpr auto to_fn_impl() const {
    auto p1_impl = p1.to_fn();
    auto p2_impl = p2.to_fn();

    return [p1_impl, p2_impl](std::istream &inp) -> Result<Out, ParseError> {
      auto pos = inp.tellg();
      auto res1 = p1_impl(inp);
      if (res1.has_val)
        return res1.ok;
      else if (res1.err.get_kind() == ParseError::ErrorVariant::Irrecoverable) {
        inp.seekg(pos);
        inp.clear();
        return res1.err;
      }
      inp.seekg(pos);
      inp.clear();
      auto res2 = p2_impl(inp);
      if (res2.has_val) {
        return res2.ok;
      } else {
        inp.seekg(pos);
        inp.clear();
        return res2.err;
      }
    };
  }
};

template <typename P, typename F,
          typename Out = std::invoke_result_t<F, typename P::value_type>>
struct Map : public Parser<Map<P, F>, Out> {
  using Parser<Map<P, F, Out>, Out>::Parser;

  const P p;
  const F f;
  constexpr Map(P const &parser, F const &f) : p(parser), f(f) {};

  constexpr auto to_fn_impl() const {
    auto p_impl = p.to_fn();
    auto fn = f;
    return [p_impl, fn](std::istream &inp) -> Result<Out, ParseError> {
      auto pos = inp.tellg();
      auto res = p_impl(inp);
      if (!res.has_val) {
        inp.seekg(pos);
        inp.clear();
        return res.err;
      }
      try {
        return fn(res.ok);
      } catch (std::exception &) {
        inp.seekg(pos);
        inp.clear();
        return ParseError("Map failed", ParseError::ErrorVariant::Recoverable,
                          pos);
      }
    };
  }
};
template <typename P, typename F,
          typename Err = std::invoke_result_t<F, ParseError>>
struct Context : public Parser<Context<P, F>, typename P::value_type> {
  using Parser<Context<P, F, Err>, typename P::value_type>::Parser;

  const P p;
  const F f;
  constexpr Context(P const &parser, F const &f) : p(parser), f(f) {};

  constexpr auto to_fn_impl() const {
    auto p_impl = p.to_fn();
    auto fn = f;
    return [p_impl,
            fn](std::istream &inp) -> Result<typename P::value_type, Err> {
      auto pos = inp.tellg();
      auto res = p_impl(inp);
      if (res.has_val)
        return res.ok;
      inp.seekg(pos);
      inp.clear();
      try {
        return fn(res.err);
      } catch (std::exception &) {
        return ParseError("Context failed",
                          ParseError::ErrorVariant::Recoverable, inp.tellg());
      }
    };
  }
};

template <typename P, typename Out> struct Many1 : Parser<Many1<P, Out>, Out> {
  using Parser<Many1<P, Out>, Out>::Parser;

  const P p;
  constexpr Many1(P const &p) : p(p) {};

  constexpr auto to_fn_impl() const {
    auto p_impl = p.to_fn();

    return [p_impl](std::istream &inp) -> Result<Out, ParseError> {
      auto pos_1 = inp.tellg();
      auto fst = p_impl(inp);
      if (!fst.has_val) {
        inp.seekg(pos_1);
        inp.clear();
        return fst.err;
      }
      Out tgt = fst.ok;
      while (true) {
        auto pos = inp.tellg();
        auto nxt = p_impl(inp);
        if (!nxt.has_val) {
          inp.seekg(pos);
          inp.clear();
          if (nxt.err.get_kind() == ParseError::ErrorVariant::Irrecoverable) {
            return nxt.err;
          }
          return tgt;
        }
        Out nxt_tgt = nxt.ok;

        std::copy(std::begin(nxt_tgt), std::end(nxt_tgt),
                  std::back_inserter(tgt));
      }
    };
  }
};

template <typename F>
struct Const : public Parser<Const<F>, typename std::invoke_result_t<F>> {
  using Out = typename std::invoke_result_t<F>;
  const F res_fn;
  constexpr Const(F const &res_fn) : res_fn(res_fn) {};

  constexpr auto to_fn_impl() const {
    const auto res_fn = this->res_fn;

    return [res_fn](std::istream &) -> Result<Out, ParseError> {
      return res_fn();
    };
  }
};

template <typename P, typename Out = typename P::value_type>
struct Cut : public Parser<Cut<P, Out>, Out> {
  const P p;
  constexpr Cut(P const &p) : p(p) {};
  constexpr auto to_fn_impl() const {
    auto p_impl = p.to_fn();
    return [p_impl](std::istream &inp) -> Result<Out, ParseError> {
      auto res = p_impl(inp);
      if (res.has_val)
        return res.ok;
      ParseError err = res.err;
      return ParseError(err.get_why(), ParseError::ErrorVariant::Irrecoverable,
                        err.get_pos());
    };
  }
};

template <typename P> struct Peek : public Parser<Peek<P>, std::tuple<>> {
  const P p;
  constexpr Peek(P const &p) : p(p) {};
  constexpr auto to_fn_impl() const {
    auto p_impl = p.to_fn();
    return [p_impl](std::istream &inp) -> Result<std::tuple<>, ParseError> {
      auto pos = inp.tellg();
      auto res = p_impl(inp);
      if (res.has_val) {
        inp.seekg(pos);
        inp.clear();
        return std::tuple<>{};
      }
      return res.err;
    };
  }
};

struct Take : public Parser<Take, std::string> {
  const uint32_t cnt;
  constexpr Take(uint32_t const &cnt) : cnt(cnt) {};
  constexpr auto to_fn_impl() const {
    auto cnt = this->cnt;
    return [cnt](std::istream &inp) -> Result<std::string, ParseError> {
      auto pos = inp.tellg();
      char *buf = new char[cnt + 1];
      memset(buf, 0, cnt + 1);
      inp.read(buf, cnt);
      if (cnt != inp.gcount()) {
        inp.clear();
        inp.seekg(pos);
        inp.clear();
        return ParseError("Could not take " + std::to_string(cnt) +
                              " characters!",
                          ParseError::ErrorVariant::Recoverable, pos);
      }
      auto res = std::string(buf);
      delete[] buf;
      return res;
    };
  }
};

template <typename Res> consteval auto constant(Res const &res) {
  auto lambda = [res]() { return res; };
  return Const<decltype(lambda)>{lambda};
}

template <typename Default> consteval auto constant() {
  auto lambda = []() { return Default{}; };
  return Const<decltype(lambda)>{lambda};
}

template <typename P1, typename P2>
consteval auto operator>>(P1 const &p1, P2 const &p2) {
  return Sequence(p1, p2);
}

template <typename P1, typename F>
consteval auto map(P1 const &p1, F const &f) {
  return Map(p1, f);
}

template <typename P, typename F>
consteval auto context(P const &p, F const &f) {
  return Context(p, f);
}

template <typename F> consteval auto verify(F const &fun) {
  return VerifyP(fun);
}

template <typename Out = char> consteval auto char_p(char tgt) {
  return map(verify([tgt](char const &val) { return val == tgt; }),
             [](char res) -> Out { return {res}; });
}

template <typename P1, typename P2>
consteval auto operator+(P1 const &p1, P2 const &p2) {
  return map(Sequence{p1, p2},
             [](auto tup) { return std::get<0>(tup) + std::get<1>(tup); });
}

template <typename P1, typename P2>
consteval auto operator|(P1 const &p1, P2 const &p2) {
  return Alternative{p1, p2};
}

template <typename P, typename Out = typename P::value_type>
consteval auto many1(P const &parser) {
  return Many1<P, Out>{parser};
}

template <typename P, typename Out>
consteval auto many0(P const &parser, Out const &default_value) {
  return many1(parser) | constant(default_value);
}

template <typename P> consteval auto many0(P const &parser) {
  return many1(parser) | constant<typename P::value_type>();
}

template <typename P> consteval auto maybe(P const &p) {
  return p | constant<typename P::value_type>();
}

template <typename P> consteval auto cut(P const &p) { return Cut<P>(p); }

template <typename P> consteval auto peek(P const &p) { return Peek<P>(p); }

consteval auto take(uint32_t const &cnt) { return Take(cnt); }

consteval auto string_p(std::string_view const &inp) {
  return map(take(inp.size()), [inp](auto s) {
    if (s == inp) {
      return s;
    } else {
      throw std::exception();
    }
  });
}

template <typename Pars> consteval auto parse_fn(Pars const &parser) {
  return parser.to_fn();
}

constexpr bool is_digit(char const &c) { return std::isdigit(c); }
constexpr bool is_alpha(char const &c) { return std::isalpha(c); }
constexpr bool is_alphanumeric(char const &c) { return std::isalnum(c); }
constexpr bool is_whitespace(char const &c) { return std::isspace(c); }
constexpr bool is_hex_digit(char const &c) {
  return ('0' <= c && '9' >= 'c') || ('A' <= c && 'F' >= c) ||
         ('a' <= c && 'f' >= c);
}

consteval auto digit() {
  return map(verify(&is_digit), [](char const &c) { return std::string{c}; });
}

consteval auto digits1() { return many1(digit()); }
consteval auto digits0() { return many0(digit()); }

consteval auto alphabetic() {
  return map(verify(&is_alpha), [](char const &c) { return std::string{c}; });
}
consteval auto alphabetics1() { return many1(alphabetic()); }
consteval auto alphabetics0() { return many0(alphabetic()); }

consteval auto alphanumeric() {
  return map(verify(&is_alphanumeric),
             [](char const &c) { return std::string{c}; });
}
consteval auto alphanumerics1() { return many1(alphanumeric()); }
consteval auto alphanumerics0() { return many0(alphanumeric()); }

consteval auto hex_digit() {
  return map(verify(&is_hex_digit),
             [](char const &c) { return std::string{c}; });
}

consteval auto hex_digits1() { return many1(hex_digit()); }
consteval auto hex_digits0() { return many0(hex_digit()); }

consteval auto ws0() {
  return many0(
      map(verify(&is_whitespace), [](auto c) { return std::string{c}; }));
}
consteval auto ws1() {
  return many1(
      map(verify(&is_whitespace), [](auto c) { return std::string{c}; }));
}

} // namespace parsum
#endif
