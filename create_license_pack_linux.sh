#!/bin/bash
SRCDIR=$(readlink -f $(dirname "$0"))
mkdir -p licenses/linux
cd licenses/linux
mkdir Boost glfw gli glm TBB vulkan-sdk zlib libc++ libstdc++ libgcc llvm-runtime g++ gcc-base gtest gmock vkglformat
cp /usr/local/share/doc/boost/LICENSE_1_0.txt Boost
cp /usr/share/doc/libglfw3/copyright glfw
cp /usr/local/share/doc/gli/manual.md gli
cp /usr/local/share/doc/gli/readme.md gli
cp /usr/local/share/doc/glm/manual.md glm
cp /usr/local/share/doc/glm/readme.md glm
cp /usr/local/share/doc/tbb/* TBB
cp /usr/local/share/doc/vulkan-sdk/LICENSE.txt vulkan-sdk
cp /usr/local/share/doc/vulkan-sdk/sdk_license.html vulkan-sdk
cp /usr/share/doc/zlib1g-dev/copyright zlib
cp /usr/share/doc/libc++-dev/copyright libc++
cp /usr/share/doc/libstdc++-8-dev/copyright libstdc++
cp /usr/share/doc/libgcc-8-dev/copyright libgcc
cp /usr/share/doc/llvm-*-runtime/copyright llvm-runtime
cp /usr/share/doc/g++-8/copyright g++
cp /usr/share/doc/gcc-8-base/copyright gcc-base
cp /usr/local/share/doc/googletest/LICENSE gtest
cp /usr/local/share/doc/googletest/CONTRIBUTORS gtest
cp /usr/local/share/doc/googlemock/LICENSE gmock
cp /usr/local/share/doc/googlemock/CONTRIBUTORS gmock
cp $SRCDIR/vkglformat/LICENSE  vkglformat
cp $SRCDIR/vkglformat/Source.txt  vkglformat
