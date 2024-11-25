#include "manifest_parser_shadowDash.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "state.h"  // Definitions for Pool, Rule, Build, etc.
#include "graph.h"
#include "state.h"
#include "util.h"
#include "version.h"

#include "shadowdash/manifest.h"  // Declarations of InitializeManifest, GetPools, GetRules, GetBuilds

bool ManifestParser::ParseManifestFromObjects(std::string* err) {
    // Initialize the manifest, which sets up pools, rules, and builds in memory
    shadowdash::InitializeManifest();

    // Load and parse pool objects
    auto pools = shadowdash::GetPools();
    for (const auto& pool_obj : pools) {
        // Extract depth and other necessary details from each pool
        // In this case, we assume pool_ has a single depth binding
        int depth = std::stoi(pool_obj.depth_.tokens_.begin()->value_.data());
        std::string name = /* Extract name from `pool_obj` in your desired way */;
        
        // Add each pool to the internal state
        if (state_->LookupPool(name) != nullptr) {
            *err = "duplicate pool '" + name + "'";
            return false;
        }
        state_->AddPool(Pool(name, depth));
    }

    // Load and parse rule objects
    auto rules = shadowdash::GetRules();
    for (const auto& rule_obj : rules) {
        std::string command;
        std::string pool;

        // Iterate over each binding in the rule object
        for (const auto& binding : rule_obj.bindings_) {
            if (binding.first == "command") {
                command = binding.second.tokens_.begin()->value_;  // Assuming single-token command strings
            } else if (binding.first == "pool") {
                pool = binding.second.tokens_.begin()->value_;
            }
        }
        
        // Add rule to the internal state
        Rule rule{name};
        rule.AddBinding("command", command);
        if (!pool.empty()) {
            rule.AddBinding("pool", pool);
        }
        env_->AddRule(rule);
    }

        // Load and parse build objects
        auto builds = shadowdash::GetBuilds();

        bool ManifestParser::ParseEdge(string* err) {
    vector<EvalString> ins, outs, validations;

    {
        EvalString out;
        if (!lexer_.ReadPath(&out, err))
        return false;
        while (!out.empty()) {
        outs.push_back(out);

        out.Clear();
        if (!lexer_.ReadPath(&out, err))
            return false;
        }
    }

    // Add all implicit outs, counting how many as we go.
    int implicit_outs = 0;
    if (lexer_.PeekToken(Lexer::PIPE)) {
        for (;;) {
        EvalString out;
        if (!lexer_.ReadPath(&out, err))
            return false;
        if (out.empty())
            break;
        outs.push_back(out);
        ++implicit_outs;
        }
    }

    if (outs.empty())
        return lexer_.Error("expected path", err);

    if (!ExpectToken(Lexer::COLON, err))
        return false;

    string rule_name;
    if (!lexer_.ReadIdent(&rule_name))
        return lexer_.Error("expected build command name", err);

    const Rule* rule = env_->LookupRule(rule_name);
    if (!rule)
        return lexer_.Error("unknown build rule '" + rule_name + "'", err);

    for (;;) {
        // XXX should we require one path here?
        EvalString in;
        if (!lexer_.ReadPath(&in, err))
        return false;
        if (in.empty())
        break;
        ins.push_back(in);
    }

    // Add all implicit deps, counting how many as we go.
    int implicit = 0;
    if (lexer_.PeekToken(Lexer::PIPE)) {
        for (;;) {
        EvalString in;
        if (!lexer_.ReadPath(&in, err))
            return false;
        if (in.empty())
            break;
        ins.push_back(in);
        ++implicit;
        }
    }

    // Add all order-only deps, counting how many as we go.
    int order_only = 0;
    if (lexer_.PeekToken(Lexer::PIPE2)) {
        for (;;) {
        EvalString in;
        if (!lexer_.ReadPath(&in, err))
            return false;
        if (in.empty())
            break;
        ins.push_back(in);
        ++order_only;
        }
    }

    // Add all validations, counting how many as we go.
    if (lexer_.PeekToken(Lexer::PIPEAT)) {
        for (;;) {
        EvalString validation;
        if (!lexer_.ReadPath(&validation, err))
            return false;
        if (validation.empty())
            break;
        validations.push_back(validation);
        }
    }

    if (!ExpectToken(Lexer::NEWLINE, err))
        return false;


    for (const auto& build_obj : builds) {
        // Extract outputs, inputs, rule, and bindings
        std::vector<std::string> outputs, inputs;
        
        for (const auto& output : build_obj.outputs_.values_) {
            outputs.push_back(output.tokens_.begin()->value_);
        }

        for (const auto& input : build_obj.inputs_.values_) {
            inputs.push_back(input.tokens_.begin()->value_);
        }

        std::string rule_name = /* Extract rule name from build_obj */;
        
        // Validate the rule existence in state
        const Rule* rule = env_->LookupRule(rule_name);
        if (!rule) {
            *err = "unknown build rule '" + rule_name + "'";
            return false;
        }

        // Add build edges and other attributes to the internal state
        Edge* edge = state_->AddEdge(rule);

        for (const auto& out : outputs) {
            state_->AddOut(edge, out, /*slash_bits*/ 0, err);
        }

        for (const auto& in : inputs) {
            state_->AddIn(edge, in, /*slash_bits*/ 0);
        }
    }

    return true;
}