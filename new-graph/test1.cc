#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>

#include "build.h"
#include "build_log.h"
#include "deps_log.h"
#include "disk_interface.h"
#include "graph.h"
#include "state.h"
#include "status.h"
#include "util.h"
using namespace std;

void CreateHelloWorldGraph(State* state) {
  BindingEnv env;
  env.AddBinding("in", "hello_world.cpp");
  env.AddBinding("out", "hello_world");
  env.AddBinding("DEP_FILE", "hello_world.d");

  // Create rules
  Rule* cxx_compiler = new Rule("CXX_COMPILER__hello_world_");

  // Create command
  EvalString command;
  command.AddText("g++ -std=gnu++11 -MMD -MF ");
  command.AddSpecial("$in");
  command.AddText(" -o ");
  command.AddSpecial("$out");
  cxx_compiler->AddBinding("command", command);

  // Create description
  EvalString description;
  description.AddText("Building CXX object ");
  description.AddSpecial("$out");
  cxx_compiler->AddBinding("description", description);

  // Add dependency file
  EvalString depfile;
  depfile.AddSpecial("$DEP_FILE");
  cxx_compiler->AddBinding("depfile", depfile);

  // Add dependency format
  EvalString deps;
  deps.AddText("g++");
  cxx_compiler->AddBinding("deps", deps);

  // Add rule to state
  state->bindings_.AddRule(cxx_compiler);

  Rule* cxx_linker = new Rule("CXX_EXECUTABLE_LINKER__hello_world_");
  EvalString link;
  link.AddText("g++ ");
  link.AddSpecial("$in");
  link.AddText(" -o ");
  link.AddSpecial("$out");
  cxx_linker->AddBinding("command", link);
  EvalString linkDescription;
  description.AddText("Linking CXX object ");
  description.AddSpecial("$out");
  cxx_linker->AddBinding("description", linkDescription);
  state->bindings_.AddRule(cxx_linker);

  // Create nodes
  Node* source_file = state->GetNode("hello_world.cpp", 0);
  Node* object_file =
      state->GetNode("CMakeFiles/hello_world.dir/hello_world.cpp.o", 0);
  Node* executable = state->GetNode("hello_world", 0);

  // Create edges
  // cout << "Binding 'in': " << env.LookupVariable("in") << endl;
  // cout << "Binding 'out': " << env.LookupVariable("out") << endl;
  // cout << "Binding 'DEP_FILE': " << env.LookupVariable("DEP_FILE") << endl;
  string error_message;
  string* err_ptr = &error_message;
  Edge* compile_edge = state->AddEdge(cxx_compiler);
  compile_edge->env_ = &env;
  if (!state->AddOut(compile_edge, object_file->path(), 0, err_ptr)) {
    // Handle error
    *err_ptr = "Failed to add output to compile edge\n";
    Error(error_message.c_str());
    return;
  }
  compile_edge->inputs_.push_back(source_file);

  Edge* link_edge = state->AddEdge(cxx_linker);
  if (!state->AddOut(link_edge, executable->path(), 0, err_ptr)) {
    // Handle error
    *err_ptr = "Failed to add output to link edge\n";
    Error(error_message.c_str());
    return;
  }
  link_edge->inputs_.push_back(object_file);

  // Set up environment for compile edge
  if (compile_edge->env_ == nullptr) {
    compile_edge->env_ = new BindingEnv(&state->bindings_);
  }
  compile_edge->env_->AddBinding(
      "DEP_FILE", "CMakeFiles/hello_world.dir/hello_world.cpp.o.d");

  // Add default target
  if (!state->AddDefault(executable->path(), err_ptr)) {
    // Handle error
    *err_ptr = "Failed to add default target: \n";
    Error(error_message.c_str());
    return;
  }
}

void CreateLogFile(const string& path) {
  ifstream infile(path);
  if (!infile.good()) {
    ofstream outfile(path);
    if (outfile.is_open()) {
      cout << "Log file created: " << path << endl;
    } else {
      cerr << "Failed to create log file: " << path << endl;
    }
  } else {
    cout << "Log file already exists: " << path << endl;
  }
}

void Error(const string& message) {
  cerr << "Error: " << message << endl;
}

int main() {
  cout << "Build start" << endl;

  State state;
  // Create Graph
  CreateHelloWorldGraph(&state);

  // Config with Verbose output and 1 process
  BuildConfig config;
  config.verbosity = config.VERBOSE;
  config.parallelism = 1;  // 1 process

  BuildLog build_log;
  DepsLog deps_log;
  RealDiskInterface disk_interface;
  Status* status = Status::factory(config);

  // Create log file
  const string log_path = "build_log";
  string build_message;
  CreateLogFile(log_path);
  const string deps_path = "deps_log";
  string deps_message;
  CreateLogFile(deps_path);

  if (!build_log.Load(log_path, &build_message)) {
    Error("Failed to load build log");
    delete status;
    return 1;
  }

  if (!deps_log.Load(deps_path, &state, &deps_message)) {
    Error("Failed to load dependencies log");
    delete status;
    return 1;
  }

  // Get currrent timestamp（ms）
  int64_t start_time_millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count();

  // Create the Builder object
  Builder builder(&state, config, &build_log, &deps_log, &disk_interface,
                  status, start_time_millis);

  string err;
  if (!builder.AddTarget("hello_world", &err)) {
    Error("Failed to add build target: " + err);
    delete status;
    return 1;
  }

  if (!builder.Build(&err)) {
    Error("Build failed: " + err);
    delete status;
    return 1;
  }

  cout << "Build completed" << endl;

  delete status;
  return 0;
}
