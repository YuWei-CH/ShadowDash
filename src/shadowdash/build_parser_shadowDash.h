#pragma once

#include <string>
#include "state.h"
#include "manifest.h"

class BindingEnv;

/**
 * @brief Parses a build object and adds it to the build graph in the given state.
 *
 * @param build_obj The shadowdash::build object to be parsed.
 * @param state The State object representing the build graph.
 * @param env The environment for variable bindings and evaluations.
 * @param err A pointer to a string where error messages are stored.
 * @return true if the parsing was successful, false otherwise.
 */
bool ParseBuild(
    const shadowdash::build& build_obj, 
    State* state, 
    BindingEnv* env, 
    std::string* err
);

/**
 * @brief Evaluates a shadowdash::str object into a resolved string.
 *
 * @param value The shadowdash::str object to be evaluated.
 * @param env The environment for variable bindings and evaluations.
 * @return The resolved string.
 */
std::string EvaluateStr(const shadowdash::str& value, BindingEnv* env);