#include "manifest.h"
#include "ninja.h"

// using namespace std;

using namespace shadowdash;
std::vector<build> manifest();

int main(int argc, char** argv) {
  const char* ninja_command = argv[0];

  cout << "Build start" << endl;

  std::vector<build> builds = manifest();

  // Config with Verbose output and 1 process
  BuildConfig config;
  config.verbosity = config.VERBOSE;
  config.parallelism = 1;  // 1 process
  State state;
  NinjaMain ninja(ninja_command, config);

  // // Create Graph
  // CreateHelloWorldGraph(&state, ninja);

  // Read build
  for (const build& b : builds) {
    const str& first_output = b.outputs_[0];

    const Token& first_output_token = *first_output.tokens_.begin();
    StringPiece output_piece(first_output_token.value_.data(),
                             first_output_token.value_.size());
    // Get the first token in first_output
    std::cout << "First output token value: " << output_piece.AsString()
              << std::endl;

    rule& rule_ = b.rule_;

    const str& first_input = b.inputs_[0];
    const Token& first_input_token = *first_input.tokens_.begin();
    StringPiece input_piece(first_input_token.value_.data(),
                            first_input_token.value_.size());
    std::cout << "First input token value: " << input_piece.AsString()
              << std::endl;

    shadowdash::map bindings_ = b.bindings_;  // TODO: add to value

    Rule* new_rule = new Rule(rule_.name_);

    // Retrieve Token
    for (const auto& binding : rule_.bindings_) {
      EvalString* value = new EvalString;
      for (shadowdash::Token token : binding.second.tokens_) {
        StringPiece piece(token.value_.data(), token.value_.size());
        if (token.type_ == Token::Type::LITERAL)
          value->AddText(piece);
        else if (token.type_ == Token::Type::VAR)
          if (token.value_ == "in"_v.value_)
            value->AddSpecial(input_piece);
          else if (token.value_ == "out"_v.value_)
            value->AddSpecial(output_piece);
        value->AddSpecial(piece);
      }
      new_rule->AddBinding(string(binding.first), *value);
    }

    state.bindings_.AddRule(new_rule);
  }

  Status* status = Status::factory(config);
  int result = ninja.RunBuild(argc - 1, &argv[1], status);

  cout << "Build completed" << endl;
  delete status;
  return 0;
}
