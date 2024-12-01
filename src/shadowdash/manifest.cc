#include "manifest.h"

using namespace shadowdash;

void manifest() {
    // Define variables
    let(flags, "-O3");

    let(pool_depth, "4");

    // Define a pool
    auto heavy_object_pool = pool_(bind("depth", str{ "pool_depth" }));

    // Define rules
    auto compile = rule(
        "compile",
        {
            bind("command", str{ Token("g++"), Token("flags"), Token("-c"), in, Token("-o"), out }),
            bind("pool", str{ "heavy_object_pool" })
        }
    );

    auto link = rule(
        "link",
        {
            bind("command", str{ "g++", in, "-o", out })
        }
    );

    // Define builds
    auto build_c = build(
        list{ str{ "hello.o" } },   // Outputs
        {},                         // Implicit Outputs
        compile,                    // Rule
        list{ str{ "hello.cc" } },  // Inputs
        {},                         // Implicit Inputs
        {},                         // Order-Only Inputs
        {
            bind("flags", str{ "-O2" }),
            bind("pool", str{ "console" })
        }                           // Bindings
    );

    auto build_l = build(
        list{ str{ "hello" } },     // Outputs
        {},                         // Implicit Outputs
        link,                       // Rule
        list{ str{ "hello.o" } },   // Inputs
        {},                         // Implicit Inputs
        {},                         // Order-Only Inputs
        {}                          // Bindings
    );

    auto build_p = build(
        list{ str{ "dummy" } },     // Outputs
        {},                         // Implicit Outputs
        phony,                      // Rule
        {},                         // Inputs
        {},                         // Implicit Inputs
        {},                         // Order-Only Inputs
        {}                          // Bindings
    );

    // Define default targets
    default_(str{ "hello" });
    default_(list{ str{ "foo1" }, str{ "foo2" } });
}