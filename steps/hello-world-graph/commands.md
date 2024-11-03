/home/yuwei/Documents/ShadowDash/src

export LD_LIBRARY_PATH=/home/yuwei/Documents/ShadowDash/build:$LD_LIBRARY_PATH

#for linux
g++ -I /home/yuwei/Documents/ShadowDash/src -L /home/yuwei/Documents/ShadowDash/build -lninja_shared -o test1 test1.cc
#below is for mac
g++-11 -I /Users/waynewang/ShadowDash/src -L /Users/waynewang/ShadowDash/build -lninja_shared -Wl,-rpath,. -o test1 test1.cc
or
g++ -I /home/yuwei/Documents/ShadowDash/src -L /home/yuwei/Documents/ShadowDash/build -lninja_shared -Wl,-rpath,/home/yuwei/Documents/ShadowDash/build -o test1 test1.cc

g++ eval_env.cc test1.cc state.cc graph.cc -o exe.out

### SO
g++ -fPIC -shared -I../../src ../../src/build_log.cc \
    ../../src/build.cc \
    ../../src/clean.cc \
    ../../src/clparser.cc \
    ../../src/dyndep.cc \
    ../../src/dyndep_parser.cc \
    ../../src/debug_flags.cc \
    ../../src/deps_log.cc \
    ../../src/disk_interface.cc \
    ../../src/edit_distance.cc \
    ../../src/elide_middle.cc \
    ../../src/eval_env.cc \
    ../../src/graph.cc \
    ../../src/graphviz.cc \
    ../../src/json.cc \
    ../../src/line_printer.cc \
    ../../src/manifest_parser.cc \
    ../../src/metrics.cc \
    ../../src/missing_deps.cc \
    ../../src/parser.cc \
    ../../src/state.cc \
    ../../src/status_printer.cc \
    ../../src/string_piece_util.cc \
    ../../src/util.cc \
    ../../src/subprocess-posix.cc \
    ../../src/lexer.cc \
    ../../src/depfile_parser.cc \
    ../../src/version.cc -o libninja.so
g++ -fPIC test1.cc -I../../src -L./ -lninja -Wl,-rpath=. -o test1

#wayne using
g++-11 -fPIC test1.cc -I../../src -L./ -lninja -Wl,-rpath=. -o test1

### For debug
g++ -g test1.cc -I../../src -L./ -lninja -Wl,-rpath=. -o test1
gdb ./test1
sharedlibrary libninja.so
run
backtrace
break xx