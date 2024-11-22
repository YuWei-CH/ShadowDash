#include "ninja.h"

using namespace std;

void CreateHelloWorldGraph(State* state, NinjaMain& ninja) {
  BindingEnv env;
  env.AddBinding("inputFile", "hello_world.cpp");
  env.AddBinding("linkFile", "hello_world.o");
  env.AddBinding("outputFile", "hello_world");

  // Create rules
  Rule* cxx_compiler = new Rule("CXX_COMPILER__hello_world_");

  // Create command
  EvalString command;
  command.AddText("g++");
  command.AddText(" ");
  command.AddText("-c");
  command.AddText(" ");
  command.AddSpecial("in");
  command.AddText(" ");
  command.AddText("-o");
  command.AddText(" ");
  command.AddSpecial("out");
  cxx_compiler->AddBinding("command", command);

  // Create description
  EvalString description;
  description.AddText("Building CXX object");
  description.AddText(" ");
  description.AddSpecial("out");
  cxx_compiler->AddBinding("description", description);

  // Add rule to state
  state->bindings_.AddRule(cxx_compiler);

  Rule* cxx_linker = new Rule("CXX_EXECUTABLE_LINKER__hello_world_");
  EvalString link;
  link.AddText("g++ ");
  link.AddText(" ");
  link.AddSpecial("in");
  link.AddText(" ");
  link.AddText(" -o ");
  link.AddText(" ");
  link.AddSpecial("out");
  cxx_linker->AddBinding("command", link);

  // Link description
  EvalString linkDescription;
  linkDescription.AddText("Linking CXX object");
  linkDescription.AddText(" ");
  linkDescription.AddSpecial("$out");
  cxx_linker->AddBinding("description", linkDescription);

  state->bindings_.AddRule(cxx_linker);

  // Create executable node
  Node* executable = state->GetNode(env.LookupVariable("outputFile"), 0);

  // Create compilation edges
  string err;
  Edge* compile_edge = ninja.GetState().AddEdge(cxx_compiler);

  // Initialize the environment and bind the necessary variables.
  if (compile_edge->env_ == nullptr) {
    compile_edge->env_ = new BindingEnv(&state->bindings_);
  }
  compile_edge->env_->AddBinding("in", env.LookupVariable("inputFile"));
  compile_edge->env_->AddBinding("out", env.LookupVariable("linkFile"));

  if (!ninja.state_.AddOut(compile_edge, env.LookupVariable("linkFile"), 0,
                           &err)) {
    Error(("Failed to add output to compile edge: " + err).c_str());
    return;
  }
  ninja.state_.AddIn(compile_edge, env.LookupVariable("inputFile"), 0);

  // Create link edges
  Edge* link_edge = ninja.GetState().AddEdge(cxx_linker);

  if (link_edge->env_ == nullptr) {
    link_edge->env_ = new BindingEnv(&state->bindings_);
  }
  link_edge->env_->AddBinding("in", env.LookupVariable("linkFile"));
  link_edge->env_->AddBinding("out", env.LookupVariable("outputFile"));

  if (!ninja.state_.AddOut(link_edge, env.LookupVariable("outputFile"), 0,
                           &err)) {
    Error(("Failed to add output to link edge: " + err).c_str());
    return;
  }
  ninja.state_.AddIn(link_edge, env.LookupVariable("linkFile"), 0);

  // Add default target
  if (!state->AddDefault(env.LookupVariable("outputFile"), &err)) {
    Error(("Failed to add default target: " + err).c_str());
    return;
  }
}

void Error(const string& message) {
  cerr << "Error: " << message << endl;
}

int main(int argc, char** argv) {
  const char* ninja_command = argv[0];

  cout << "Build start" << endl;

  // Config with Verbose output and 1 process
  BuildConfig config;
  config.verbosity = config.VERBOSE;
  config.parallelism = 1;  // 1 process
  State state;
  NinjaMain ninja(ninja_command, config);

  // Create Graph
  CreateHelloWorldGraph(&state, ninja);

  Status* status = Status::factory(config);
  int result = ninja.RunBuild(argc - 1, &argv[1], status);

  cout << "Build completed" << endl;
  delete status;
  return 0;
}
