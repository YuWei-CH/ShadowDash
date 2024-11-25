#include "build_parser_shadowDash.h"
#include "util.h"

bool ParseBuild(
    const shadowdash::build& build_obj, 
    State* state, 
    BindingEnv* env,  // This parameter could be removed if no variables exist
    std::string* err
) {
    // Step 1: Extract Outputs
    if (build_obj.outputs_.size() == 0) {
        *err = "expected at least one output";
        return false;
    }

    Edge* edge = state->AddEdge(nullptr); // Create a new edge, rule will be set later.

    // Directly process outputs
    for (const auto& output : build_obj.outputs_.values_) {
        std::string output_path = std::string(output.tokens_.begin()->value_); // Use token directly
        uint64_t slash_bits;
        CanonicalizePath(&output_path, &slash_bits);
        if (!state->AddOut(edge, output_path, slash_bits, err)) {
            return false;
        }
    }

    // Directly process inputs
    for (const auto& input : build_obj.inputs_.values_) {
        std::string input_path = std::string(input.tokens_.begin()->value_); // Use token directly
        uint64_t slash_bits;
        CanonicalizePath(&input_path, &slash_bits);
        state->AddIn(edge, input_path, slash_bits);
    }

    for (const auto& binding : build_obj.bindings_) {
        std::string key(binding.first);  // Explicitly convert string_view to string
        const shadowdash::str& value = binding.second;

        // Directly concatenate the literal values
        std::string resolved_value;
        for (const auto& token : value.tokens_) {
            resolved_value += std::string(token.value_);
        }

        env->AddBinding(key, resolved_value); // Optional: If BindingEnv is retained
    }

    // Step 3: Finalize Edge
    if (edge->outputs_.empty()) {
        *err = "all outputs are already created by other edges";
        return false;
    }

    state->edges_.push_back(edge);
    return true;
}