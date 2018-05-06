mkdir licenses\windows
cd licenses\windows
mkdir Boost glfw gli glm TBB vulkan-sdk zlib mingw-w64\mingw-w64 vkglformat
copy C:\Libs\Boost\LICENSE_1_0.txt Boost
copy C:\Libs\mingw\glfw3\COPYING.txt glfw
copy C:\Libs\gli\License.txt gli
copy C:\Libs\glm\License.txt glm
copy C:\Libs\mingw\TBB\LICENSE TBB
copy %VULKAN_SDK%\LICENSE.txt vulkan-sdk
copy %VULKAN_SDK%\SDK_LICENSE.rtf vulkan-sdk
copy C:\Libs\zlib\README zlib
cmake -E copy_directory C:/mingw-w64/mingw64/licenses/winpthreads mingw-w64/winpthreads
cmake -E copy_directory C:/mingw-w64/mingw64/licenses/gcc mingw-w64/gcc
copy C:\mingw-w64\mingw64\licenses\mingw-w64\COPYING.MinGW-w64-runtime.txt mingw-w64
copy C:\Libs\gtest\LICENSE
copy C:\Libs\gtest\CONTRIBUTORS
copy ..\..\vkglformat\LICENSE vkglformat
copy ..\..\vkglformat\Source.txt vkglformat
