cmake_minimum_required (VERSION 3.10)
project (multicore_engine_tools
		VERSION ${BUILD_VERSION_MAJOR}.${BUILD_VERSION_MINOR}.${BUILD_VERSION_PATCH}.${BUILD_VERSION_TWEAK}
		# Language C is needed for phthreads detection to work correctly
		LANGUAGES CXX C
	)

include("${CMAKE_CURRENT_LIST_DIR}/../UseModules.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake_internal")

set(CMAKE_CONFIGURATION_TYPES "Release")
include(BuildConfig)

add_subdirectory(../multicore_engine_base multicore_engine_base)
add_subdirectory(../multicore_engine_load_unit_gen multicore_engine_load_unit_gen)
add_subdirectory(../multicore_engine_pack_file_gen multicore_engine_pack_file_gen)
