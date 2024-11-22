// manifest_parser.cc
#include "manifest.h"

namespace shadowdash {

// Containers to store manifest objects
std::vector<pool_> pools;
std::vector<rule> rules;
std::vector<build> builds;

// Manifest function to define and initialize all objects
void manifest() {
    // Define a pool
    auto heavy_object_pool = pool_(binding("depth", str{"4"_l}));
    pools.push_back(heavy_object_pool);

    // Define a rule with command and pool bindings
    auto compile = rule({
        bind("command", "g++ -O3 -c $in -o $out"_l),
        bind("pool", "heavy_object_pool"_v)
    });
    rules.push_back(compile);

    // Define another rule
    auto link = rule({
        bind("command", "g++ $in -o $out"_l)
    });
    rules.push_back(link);

    // Define builds with these rules
    auto build_c = build(
        list{str{"hello.o"_l}},       // Outputs
        {},                           // Implicit Outputs
        compile,                      // Rule
        list{str{"hello.cc"_l}},      // Inputs
        {},                           // Implicit Inputs
        {},                           // Order Only Inputs
        {bind("flags", "-O2"_l)}      // Bindings
    );
    builds.push_back(build_c);

    auto build_l = build(
        list{str{"hello"_l}},
        {},
        link,
        list{str{"hello.o"_l}},
        {},
        {},
        {}
    );
    builds.push_back(build_l);

    // Optionally define other build objects, rules, pools as needed
}

// Implements InitializeManifest, which calls `manifest` to set up objects
extern "C" void InitializeManifest() {
    manifest();  // Initializes pools, rules, and builds
}

// Provides access to initialized pools
extern "C" std::vector<pool_> GetPools() {
    return pools;
}

// Provides access to initialized rules
extern "C" std::vector<rule> GetRules() {
    return rules;
}

// Provides access to initialized builds
extern "C" std::vector<build> GetBuilds() {
    return builds;
}

}  // namespace shadowdash