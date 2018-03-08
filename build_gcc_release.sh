#!/bin/bash
cmake -E make_directory build_gcc_release
cd build_gcc_release
cmake -U*_INCLUDE -U*_LIBS -ULIBS_DIR -G "Unix Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j 10
echo "export LD_LIBRARY_PATH=/usr/local/lib/gcc-release/"

