#!/bin/bash
mkdir -p licenses/linux
cd licenses/linux
mkdir Boost glfw gli glm TBB vulkan-sdk zlib libc++ libstdc++ libgcc llvm-runtime g++ gcc-base gtest gmock vkglformat
cp /usr/local/share/doc/boost/LICENSE_1_0.txt Boost
cp /usr/share/doc/libglfw3/copyright glfw
cp /usr/share/doc/libgli-dev/copyright gli
cp /usr/share/doc/libglm-dev/copyright glm
cp /usr/share/doc/libtbb-dev/copyright TBB
cp /usr/local/share/doc/vulkan-sdk/LICENSE.txt vulkan-sdk
cp /usr/local/share/doc/vulkan-sdk/sdk_license.html vulkan-sdk
cp /usr/share/doc/zlib1g-dev/copyright zlib
cp /usr/share/doc/libc++-dev/copyright libc++
cp /usr/share/doc/libstdc++-6-dev/copyright libstdc++
cp /usr/share/doc/libgcc-6-dev/copyright libgcc
cp /usr/share/doc/llvm-3.8-runtime/copyright llvm-runtime
cp /usr/share/doc/g++-6/copyright g++
cp /usr/share/doc/gcc-6-base/copyright gcc-base
cp /usr/local/share/doc/googletest/LICENSE gtest
cp /usr/local/share/doc/googletest/CONTRIBUTORS gtest
cp /usr/local/share/doc/googlemock/LICENSE gmock
cp /usr/local/share/doc/googlemock/CONTRIBUTORS gmock
cp /usr/local/share/doc/vkglformat/LICENSE  vkglformat
cp /usr/local/share/doc/vkglformat/Source.txt  vkglformat