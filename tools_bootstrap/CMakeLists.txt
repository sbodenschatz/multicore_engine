cmake_minimum_required (VERSION 3.10)
project (multicore_engine_tools LANGUAGES CXX)

include("${CMAKE_CURRENT_LIST_DIR}/../UseModules.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")

set(CMAKE_CONFIGURATION_TYPES "Release")
include(BuildConfig)

add_subdirectory(../multicore_engine_parsers multicore_engine_parsers)
add_subdirectory(../multicore_engine_base multicore_engine_base)
add_subdirectory(../multicore_engine_load_unit_gen multicore_engine_load_unit_gen)
add_subdirectory(../multicore_engine_pack_file_gen multicore_engine_pack_file_gen)
