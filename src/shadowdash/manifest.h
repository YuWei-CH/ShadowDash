#pragma once

#include <initializer_list>
#include <string_view>
#include <string>
#include <vector>
#include <utility>

namespace shadowdash {

// Token class representing literals and variables
class Token {
public:
    enum Type { LITERAL, VAR };

    constexpr Token(Type type, std::string_view value) : type_(type), value_(value) {}

    constexpr Token(std::string_view value) : Token(Token::LITERAL, value) {}

    constexpr Token(const char* value) : Token(Token::LITERAL, std::string_view(value)) {}

    Type type_;
    std::string_view value_;
};

// Helper literal operators
constexpr Token operator"" _l(const char* value, std::size_t len) {
    return Token(Token::Type::LITERAL, { value, len });
}

constexpr Token operator"" _v(const char* value, std::size_t len) {
    return Token(Token::Type::VAR, { value, len });
}

// Represents a structured string, holding multiple tokens
class str {
public:
    str(std::initializer_list<Token> tokens) : tokens_(tokens) {}

    const std::initializer_list<Token>& tokens() const { return tokens_; }

private:
    std::initializer_list<Token> tokens_;
};

// Represents a key-value binding
using binding = std::pair<std::string, str>;
using map = std::vector<binding>;

// Represents a list of strings
class list {
public:
    list(std::initializer_list<str> values) : values_(values) {}

    size_t size() const { return values_.size(); }

    const str& operator[](size_t i) const { return *(values_.begin() + i); }

    const std::vector<str>& values() const { return values_; }

private:
    std::vector<str> values_;  // Changed to vector for dynamic access
};

// Represents a variable
class var {
public:
    var(const char* name, str value) : name_(name), value_(std::move(value)) {}

    const std::string& name() const { return name_; }

    const str& value() const { return value_; }

private:
    std::string name_;
    str value_;
};

// Represents a build rule
class rule {
public:
    rule(const std::string& name, map bindings) : name_(name), bindings_(std::move(bindings)) {}

    const std::string& name() const { return name_; }

    const map& bindings() const { return bindings_; }

private:
    std::string name_;
    map bindings_;
};

// Represents a resource pool
class pool_ {
public:
    pool_(binding depth) : depth_(std::move(depth)) {}

    const binding& depth() const { return depth_; }

private:
    binding depth_;
};

// Represents a build configuration
class build {
public:
    build(list outputs,
          list implicit_outputs,
          const rule& rule,
          list inputs,
          list implicit_inputs,
          list order_only_inputs,
          map bindings)
        : outputs_(std::move(outputs)),
          implicit_outputs_(std::move(implicit_outputs)),
          rule_(rule),
          inputs_(std::move(inputs)),
          implicit_inputs_(std::move(implicit_inputs)),
          order_only_inputs_(std::move(order_only_inputs)),
          bindings_(std::move(bindings)) {}

    // Accessors
    const list& outputs() const { return outputs_; }
    const list& implicit_outputs() const { return implicit_outputs_; }
    const rule& get_rule() const { return rule_; }
    const list& inputs() const { return inputs_; }
    const list& implicit_inputs() const { return implicit_inputs_; }
    const list& order_only_inputs() const { return order_only_inputs_; }
    const map& bindings() const { return bindings_; }

private:
    list outputs_;
    list implicit_outputs_;
    const rule& rule_;
    list inputs_;
    list implicit_inputs_;
    list order_only_inputs_;
    map bindings_;
};

// Represents default build targets
class default_ {
public:
    default_(list addedList) {
        for (const auto& item : addedList.values()) {
            targets_.emplace_back(item);
        }
    }

    default_(str addedTarget) {
        targets_.emplace_back(std::move(addedTarget));
    }

    static const std::vector<str>& targets() { return targets_; }

private:
    static std::vector<str> targets_;
};

// Static initialization for default targets
std::vector<str> default_::targets_;

// Helper variables
static constexpr auto in = "in"_v;
static constexpr auto out = "out"_v;
static auto phony = rule("phony", {});
static auto console = pool_(binding("depth", str{ {"1"} }));

}  // namespace shadowdash

// Macro helpers for defining variables and bindings
#define let(name, ...) \
    shadowdash::var name { \
        #name, shadowdash::str { __VA_ARGS__ } \
    }

#define bind(name, ...) \
    shadowdash::binding { \
        #name, shadowdash::str { __VA_ARGS__ } \
    }