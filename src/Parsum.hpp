#ifndef LIB_PARSUM_HPP
#define LIB_PARSUM_HPP

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <istream>
#include <iterator>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace parsum {
// Inspired by
// https://github.com/Qqwy/cpp-parser_combinators/blob/main/optimized/main.cc.

// join_tup()
template <typename T, typename U>
std::tuple<T, U> constexpr join_tup(T const &lhs, U const &rhs) {
  return std::make_tuple(lhs, rhs);
}
template <typename... Ts, typename U>
std::tuple<Ts..., U> constexpr join_tup(std::tuple<Ts...> const &lhs,
                                        U const &rhs) {
  return std::tuple_cat(lhs, std::make_tuple(rhs));
}
template <typename T, typename... Us>
std::tuple<T, Us...> constexpr join_tup(T const &lhs,
                                        std::tuple<Us...> const &rhs) {
  return std::tuple_cat(std::make_tuple(lhs), rhs);
}

template <typename... Ts, typename... Us>
std::tuple<Ts..., Us...> constexpr join_tup(std::tuple<Ts...> const &lhs,
                                            std::tuple<Us...> const &rhs) {
  return std::tuple_cat(lhs, rhs);
}
// join_tup() END

// From tup
template <typename CT, typename... Ts>
constexpr CT from_tup(std::tuple<Ts...> const &tup) {
  return std::apply([](auto... elems) { return CT{elems...}; }, tup);
}

template <typename Out, typename... Ts>
Out concat_all(Out default_val, Ts... elems) {
  return (default_val + ... + elems);
}

template <typename... Ts> std::string from_tup(std::tuple<Ts...> const &tup) {
  return std::apply(
      [](auto... elems) { return concat_all(std::string(), elems...); }, tup);
}
// From tup end

// INTO
template <typename In, typename Out> Out into(In const &val) {
  return from_tup<Out>(val);
}

template <> inline std::string into(std::vector<char> const &val) {
  return std::string(val.begin(), val.end());
}

template <> inline std::vector<char> into(std::string const &val) {
  return std::vector<char>(val.begin(), val.end());
}

template <> inline std::string into(char const &val) { return {val}; }

template <typename T> std::vector<T> into(T const &val) { return {val}; }

template <> inline std::string into(std::tuple<char, std::string> const &val) {
  auto [ch, st] = val;
  return std::string{ch} + st;
}

template <> inline std::string into(std::tuple<std::string, char> const &val) {
  auto [st, ch] = val;
  return st + std::string{ch};
}

template <> inline std::string into(std::tuple<char, char> const &val) {
  auto [ch1, ch2] = val;
  return std::string{ch1, ch2};
}

template <>
inline std::string into(std::tuple<char, char, std::string> const &val) {
  auto [ch1, ch2, st] = val;
  return std::string{ch1, ch2} + st;
}
// INTO end

// ParseError
struct ParseError {
public:
  enum class ErrorVariant {
    Recoverable,
    Irrecoverable,
  };
  constexpr ParseError(std::string const &why, ErrorVariant kind, uint64_t pos)
      : why(why), pos(pos), kind(kind) {};
  constexpr ErrorVariant get_kind() { return this->kind; }
  constexpr std::string const &get_why() const { return this->why; }
  constexpr uint64_t get_pos() const { return pos; }
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
  template <typename T>
  constexpr std::string display(T const &collection) const {
    auto [column, line] = get_coord(collection);
    return "Found error at line " + std::to_string(line) + ", column " +
           std::to_string(column) + ": " + std::string(this->why);
  }
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
      if (!inp)
        return ParseError("Reached end of file!",
                          ParseError::ErrorVariant::Recoverable, inp.tellg());
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
        return res1.err;
      }
      auto res2 = p2_impl(inp);
      if (!res2.has_val) {
        inp.seekg(pos);
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
        return res1.err;
      }
      inp.seekg(pos);
      return p2_impl(inp);
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
        return res.err;
      }
      try {
        return fn(res.ok);
      } catch (std::exception &) {
        inp.seekg(pos);
        return ParseError("Map failed", ParseError::ErrorVariant::Recoverable,
                          inp.tellg());
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
        return fst.err;
      }
      Out tgt = fst.ok;
      while (true) {
        auto pos = inp.tellg();
        auto nxt = p_impl(inp);
        if (!nxt.has_val) {
          inp.seekg(pos);
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
        return std::tuple<>{};
      }
      return res.err;
    };
  }
};

// template <typename A, typename F,
//           typename Out = std::invoke_result_t<typename F::value_type,
//                                               typename A::value_type>>
// struct Applicative : Parser<Applicative<A, F, Out>, Out> {
//   const A p1;
//   const F p2;
//   constexpr Applicative(A const &p1, F const &p2) : p1(p1), p2(p2) {};
//
//   constexpr auto to_fn_impl() const {
//     auto p1_impl = p1.to_fn();
//     auto p2_impl = p2.to_fn();
//
//     return [p1_impl, p2_impl](std::istream &inp) -> Result<Out,
//     ParseError> {
//       auto val_r = p1_impl(&inp);
//     };
//   }
// };
//

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

template <typename Pars> consteval auto parse_fn(Pars const &parser) {
  return parser.to_fn();
}

consteval auto digit() {
  return map(verify([](char const &c) { return isdigit(c); }),
             [](char const &c) { return std::string{c}; });
}

consteval auto ws0() {
  return many0(map(verify([](const char &c) { return isspace(c); }),
                   [](auto c) { return std::string{c}; }));
}
consteval auto ws1() {
  return many1(map(verify([](const char &c) { return isspace(c); }),
                   [](auto c) { return std::string{c}; }));
}

} // namespace parsum
#endif
