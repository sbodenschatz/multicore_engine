cmake_minimum_required (VERSION 2.6)
project (multicore_engine_tools)

include(../CMakeUtils.txt)
set(CMAKE_CONFIGURATION_TYPES "Release")
include(../CMakeConfig.txt)

include_directories(${CMAKE_SOURCE_DIR}/../multicore_engine_core/include)
include_directories(${CMAKE_SOURCE_DIR}/../multicore_engine_parsers/include)

add_subdirectory(../multicore_engine_parsers multicore_engine_parsers)
add_subdirectory(../multicore_engine_core multicore_engine_core)
add_subdirectory(../multicore_engine_load_unit_gen multicore_engine_load_unit_gen)
add_subdirectory(../multicore_engine_pack_file_gen multicore_engine_pack_file_gen)

