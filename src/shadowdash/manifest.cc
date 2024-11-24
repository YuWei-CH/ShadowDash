#include "manifest.h"

#include <vector>
using namespace shadowdash;

std::vector<build> manifest() {
  std::vector<build> builds;

  let(flags, "-O3");

  let(pool_depth, "4");

  auto heavy_object_pool = pool_(manifest_bind(depth, "pool_depth"_v));

  auto compile =
      rule("compile_rule",
           { manifest_bind(command, "g++", "flags"_v, "-c", in, "-o", out),
             manifest_bind(pool, "heavy_object_pool"_v) });

  auto link =
      rule("link_rule", {
                            manifest_bind(command, "g++", in, "-o", out),
                        });

  auto build_c = build(
      list{ str{ "hello.o" } }, {}, compile, list{ str{ "hello.cc" } }, {}, {},
      { manifest_bind(flags, "-O2"), manifest_bind(pool, "console"_v) });

  auto build_l = build(list{ str{ "hello" } }, {}, link,
                       list{ str{ "hello.o" } }, {}, {}, {});

  auto build_p = build(list{ str{ "dummy" } }, {}, phony, {}, {}, {}, {});

  default_(str{ "hello" });
  default_(list{ str{ "foo1" }, str{ "foo2" } });

  builds.push_back(build_c);
  builds.push_back(build_l);
  builds.push_back(build_p);

  return builds;
}