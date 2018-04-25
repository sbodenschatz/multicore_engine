cmake_minimum_required (VERSION 3.10)
project (multicore_engine_tools)

include(../cmake/Utils.cmake)
set(CMAKE_CONFIGURATION_TYPES "Release")
include(../cmake/BuildConfig.cmake)

include_directories(${CMAKE_SOURCE_DIR}/../multicore_engine_base/include)
include_directories(${CMAKE_SOURCE_DIR}/../multicore_engine_core/include)
include_directories(${CMAKE_SOURCE_DIR}/../multicore_engine_parsers/include)

message(STATUS "Branch: " ${BUILD_BRANCH})
message(STATUS "Version: " ${BUILD_VERSION})
message(STATUS "Version string: " ${BUILD_VERSION_STRING})

add_subdirectory(../multicore_engine_parsers multicore_engine_parsers)
add_subdirectory(../multicore_engine_base multicore_engine_base)
add_subdirectory(../multicore_engine_load_unit_gen multicore_engine_load_unit_gen)
add_subdirectory(../multicore_engine_pack_file_gen multicore_engine_pack_file_gen)
