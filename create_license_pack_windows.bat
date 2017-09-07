mkdir licenses\windows
cd licenses\windows
mkdir Boost glfw gli glm TBB vulkan-sdk zlib mingw-w64\mingw-w64 vkglformat
copy C:\Libs\Boost\LICENSE_1_0.txt Boost
copy C:\Libs\glfw\COPYING.txt glfw
copy C:\Libs\gli\copying.txt gli
copy C:\Libs\glm\copying.txt glm
copy C:\Libs\TBB\LICENSE TBB
copy %VULKAN_SDK%\LICENSE.txt vulkan-sdk
copy %VULKAN_SDK%\SDK_LICENSE.rtf vulkan-sdk
copy C:\Libs\zlib\README zlib
cmake -E copy_directory C:/mingw-w64/mingw64/licenses/winpthreads mingw-w64/winpthreads
cmake -E copy_directory C:/mingw-w64/mingw64/licenses/gcc mingw-w64/gcc
copy C:\mingw-w64\mingw64\licenses\mingw-w64\COPYING.MinGW-w64-runtime.txt mingw-w64
copy C:\Libs\gtest\GMOCK-LICENSE.txt
copy C:\Libs\gtest\GMOCK-CONTRIBUTORS.txt
copy C:\Libs\gtest\GTEST-LICENSE.txt
copy C:\Libs\gtest\GTEST-CONTRIBUTORS.txt
copy C:\Libs\vkglformat\LICENSE vkglformat
copy C:\Libs\vkglformat\Source.txt vkglformat
