#!/bin/bash
cmake -E make_directory build_clang_debug
cd build_clang_debug
cmake -G "Unix Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -- -j 16
echo "export LD_LIBRARY_PATH=/usr/local/lib/local-boost/lib-gcc-debug/"

