#pragma once

#include <initializer_list>
#include <string_view>
#include <string>
#include <vector>

namespace shadowdash {

class Token {
public:
  enum Type { LITERAL, VAR};

  constexpr Token(Type type, std::string_view value) : type_(type), value_(value) {}

  constexpr Token(std::string_view value) : Token(Token::LITERAL, value) {}

  constexpr Token(const char* value) : Token(Token::LITERAL, value) {}

  Type type_;
  std::string_view value_;
};

constexpr Token operator"" _l(const char* value, std::size_t len) {
  return Token(Token::Type::LITERAL, { value, len });
}

constexpr Token operator"" _v(const char* value, std::size_t len) {
  return Token(Token::Type::VAR, { value, len });
}

class str {
public:
  str(std::initializer_list<Token> tokens) : tokens_(tokens) {}
  std::initializer_list<Token> tokens_;
};

using binding = std::pair<std::string_view, str>;
using map = std::initializer_list<binding>;

class list {
public:
  list(std::initializer_list<str> values) : values_(values) {}
  constexpr size_t size() const {return values_.size();}
  str operator[] (size_t i) const {return *(values_.begin() + i);}

private:
  std::initializer_list<str> values_;
};

class var {
private:
  const char* name_;
  str value_;
public:
  var(const char* name, str value): name_(name), value_(value) {}
};

class rule {
private:
  map bindings_;
public:
  rule(map bindings): bindings_(bindings) {}
};

class pool_ {
private:
  binding depth_;
public:
  pool_(binding depth): depth_(depth)  {}
};

class build {
private:
    list outputs_;
    list implicit_outputs_;
    rule& rule_;
    list inputs_;
    list implicit_inputs_;
    list order_only_inputs_;
    map bindings_;

public:
    build(
        list outputs,
        list implicit_outputs,
        rule& rule,
        list inputs,
        list implicit_inputs,
        list order_only_inputs,
        map bindings
    ) : outputs_(outputs),
        implicit_outputs_(implicit_outputs),
        rule_(rule),
        inputs_(inputs),
        implicit_inputs_(implicit_inputs),
        order_only_inputs_(order_only_inputs),
        bindings_(bindings) {}
};

class default_{
public:
  default_(list addedList){
    for (size_t i = 0; i < addedList.size(); i++) targets_.push_back(addedList[i]);
  }

  default_(str addedTarget) {
    targets_.push_back(addedTarget);
  }

  static std::vector<str> targets_;
};

std::vector<str> default_::targets_;

static constexpr auto in = "in"_v;
static constexpr auto out = "out"_v;
static auto phony = rule{{}};
static auto console = pool_(binding("depth", str{ {"1"} }));

}

#define let(name, ...) \
  var name {           \
    #name, str {       \
      __VA_ARGS__      \
    }                  \
  }

#define bind(name, ...) \
  {                     \
    #name, str {        \
      __VA_ARGS__       \
    }                   \
  }