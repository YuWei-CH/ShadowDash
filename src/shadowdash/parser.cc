#include "parser.h"
#include "../state.h"

#include <stdexcept>
#include <string>
#include <utility>

namespace shadowdash {

void AddBuildToNinjaState(const build& b, State* ninja_state) {
    // Step 1: Lookup Rule
    const Rule* rule_ninja = ninja_state->bindings_.LookupRule(b.get_rule().name());
    if (!rule_ninja) {
        throw std::runtime_error("Rule not found: " + b.get_rule().name());
    }

    // Step 2: Create Edge
    Edge* edge = ninja_state->AddEdge(rule_ninja);

    // Step 3: Add Outputs
    std::string err;
    for (const auto& output_str : b.outputs().values()) {
        std::string output;
        for (const auto& token : output_str.tokens()) {
            output.append(token.value_);
        }
        if (!ninja_state->AddOut(edge, std::move(output), 0, &err)) {
            throw std::runtime_error("Failed to add output: " + err);
        }
    }

    // Step 4: Add Implicit Outputs
    int implicit_output_count = 0;
    for (const auto& implicit_output_str : b.implicit_outputs().values()) {
        std::string output;
        for (const auto& token : implicit_output_str.tokens()) {
            output.append(token.value_);
        }
        ++implicit_output_count;
        if (!ninja_state->AddOut(edge, std::move(output), 0, &err)) {
            throw std::runtime_error("Failed to add implicit output: " + err);
        }
    }
    edge->implicit_outs_ = implicit_output_count;

    // Step 5: Add Inputs
    for (const auto& input_str : b.inputs().values()) {
        std::string input;
        for (const auto& token : input_str.tokens()) {
            input.append(token.value_);
        }
        ninja_state->AddIn(edge, std::move(input), 0);
    }

    // Step 6: Add Implicit Inputs
    int implicit_input_count = 0;
    for (const auto& implicit_input_str : b.implicit_inputs().values()) {
        std::string input;
        for (const auto& token : implicit_input_str.tokens()) {
            input.append(token.value_);
        }
        ++implicit_input_count;
        ninja_state->AddIn(edge, std::move(input), 0);
    }
    edge->implicit_deps_ = implicit_input_count;

    // Step 7: Add Order-Only Inputs
    int order_only_count = 0;
    for (const auto& order_only_input_str : b.order_only_inputs().values()) {
        std::string input;
        for (const auto& token : order_only_input_str.tokens()) {
            input.append(token.value_);
        }
        ++order_only_count;
        ninja_state->AddIn(edge, std::move(input), 0);
    }
    edge->order_only_deps_ = order_only_count;

    // Step 8: Add Bindings
    BindingEnv* env = new BindingEnv(&ninja_state->bindings_);
    for (const auto& binding : b.bindings()) {
        EvalString value;
        for (const auto& token : binding.second.tokens()) {
            if (token.type_ == Token::LITERAL) {
                value.AddText(std::string(token.value_));
            } else if (token.type_ == Token::VAR) {
                value.AddSpecial(std::string(token.value_));
            }
        }
        env->AddBinding(std::string(binding.first), value.Evaluate(env));
    }
    edge->env_ = env;

    // Edge added successfully
}

}  // namespace shadowdash