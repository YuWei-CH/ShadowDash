#!/bin/bash

# Check OS
os_type=$(uname)
# Set default compiler and flags
export CC=gcc
export CXX=g++

if [[ "$os_type" == "Linux" ]]; then
    lib_extension="so"
    shared_flag="-shared"
    rpath_flag="-Wl,-rpath=."
elif [[ "$os_type" == "Darwin" ]]; then
    # Check gcc-11 on MacOS
    if command -v gcc-11 &> /dev/null && command -v g++-11 &> /dev/null; then
        echo "GNU g++-11 detected. Using g++-11 as the compiler."
        export CC=gcc-11
        export CXX=g++-11
    else
        echo "GNU g++-11 not found. Please install it using Homebrew with 'brew install gcc@11'."
        echo "If another version of GCC (not Clang) is installed, please update this script accordingly."
        exit 1
    fi
    lib_extension="dylib"
    shared_flag="-dynamiclib"
    
    # Add MacOS SDK path for standard headers
    export SDK_PATH=$(xcrun --sdk macosx --show-sdk-path)
    extra_flags="-isysroot $SDK_PATH"
    rpath_flag="-Wl,-rpath,@loader_path"
else
    # Assume is Windows
    lib_extension="dll"
    shared_flag="-shared"
    rpath_flag="-Wl,-rpath=."
fi

# Build dynamic library
echo "Building libninja.${lib_extension}..."
$CXX -fPIC ${shared_flag} -I../src ${extra_flags} ../src/build_log.cc \
    ../src/build.cc \
    ../src/clean.cc \
    ../src/clparser.cc \
    ../src/dyndep.cc \
    ../src/dyndep_parser.cc \
    ../src/debug_flags.cc \
    ../src/deps_log.cc \
    ../src/disk_interface.cc \
    ../src/edit_distance.cc \
    ../src/elide_middle.cc \
    ../src/eval_env.cc \
    ../src/graph.cc \
    ../src/graphviz.cc \
    ../src/json.cc \
    ../src/line_printer.cc \
    ../src/manifest_parser.cc \
    ../src/metrics.cc \
    ../src/missing_deps.cc \
    ../src/parser.cc \
    ../src/state.cc \
    ../src/status_printer.cc \
    ../src/string_piece_util.cc \
    ../src/util.cc \
    ../src/subprocess-posix.cc \
    ../src/lexer.cc \
    ../src/depfile_parser.cc \
    ../src/version.cc -o "libninja.${lib_extension}"

# Compile test1.cc
echo "Compiling test1.cc..."
$CXX -fPIC test1.cc -I../src -L./ -lninja ${rpath_flag} ${extra_flags} -o test1

# Compile hello world using test1
echo "Running test1..."
./test1 hello_world