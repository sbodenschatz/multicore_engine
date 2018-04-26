cmake_minimum_required (VERSION 3.10)
project (multicore_engine)

include("${CMAKE_CURRENT_LIST_DIR}/UseModules.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

enable_testing()

if(NOT MSVC)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
endif()

include(BuildConfig)

include(IntegrateBuildScripts)
make_build_scripts_project(MCE .gitlab-ci.yml .gitlab-ci-edges-env.yml Doxyfile .clang-format)
include(RepositoryVersionDetector)
get_version_info()

if(NOT STATIC_ANALYSIS_ONLY)
	add_subdirectory(multicore_engine_assets)
	add_subdirectory(multicore_engine_test_assets)
endif()

add_subdirectory(multicore_engine_parsers)
add_subdirectory(multicore_engine_base)
add_subdirectory(multicore_engine_core)
add_subdirectory(multicore_engine_headed)
add_subdirectory(multicore_engine_renderer)
add_subdirectory(multicore_engine_tests)
add_subdirectory(multicore_engine_graphics_test)
add_subdirectory(multicore_engine_load_unit_gen)
add_subdirectory(multicore_engine_pack_file_gen)
add_subdirectory(multicore_engine_model_converter)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT mce_graphics_test)
