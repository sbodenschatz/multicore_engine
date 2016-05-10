cmake_minimum_required (VERSION 2.6)
project (multicore_engine)

include(CMakeUtils.txt)

make_build_scripts_project(.gitlab-ci.yml Doxyfile .clang-format)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
include(CMakeConfig.txt)

if(NOT STATIC_ANALYSIS_ONLY)
	bootstrap_tools()
	add_subdirectory(multicore_engine_assets)
endif()

add_subdirectory(multicore_engine_parsers)
add_subdirectory(multicore_engine_base)
add_subdirectory(multicore_engine_core)
add_subdirectory(multicore_engine_core_tests)
add_subdirectory(multicore_engine_demo)
add_subdirectory(multicore_engine_load_unit_gen)
add_subdirectory(multicore_engine_pack_file_gen)

