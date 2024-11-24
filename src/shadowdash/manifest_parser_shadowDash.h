#pragma once

#include <string>
#include "state.h"  // For definitions of Pool, Rule, Build, etc.
#include "shadowdash/manifest.h"

class ManifestParser {
public:
    // Constructor accepting the state to populate and an optional error flag for verbosity
    ManifestParser(State* state, bool quiet = false);

    // Parses the manifest from in-memory objects initialized by InitializeManifest
    bool ParseManifestFromObjects(std::string* err);

private:
    // Reference to the Ninja build state that will be populated
    State* state_;

    // Determines whether to suppress warnings
    bool quiet_;

    // Helper functions to interpret pools, rules, and builds
    bool ParsePools(std::string* err);
    bool ParseRules(std::string* err);
    bool ParseBuilds(std::string* err);
};