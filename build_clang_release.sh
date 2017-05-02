#!/bin/bash
cmake -E make_directory build_clang_release
cd build_clang_release
cmake -U*_INCLUDE -U*_LIBS -ULIBS_DIR -G "Unix Makefiles" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -- -j 16
echo "export LD_LIBRARY_PATH=/usr/local/lib/clang-release/"

