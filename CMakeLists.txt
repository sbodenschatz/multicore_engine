cmake_minimum_required (VERSION 3.7)
project (multicore_engine)

include(CMakeUtils.txt)

enable_testing()

if(NOT MSVC)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
endif()

include(CMakeConfig.txt)

make_build_scripts_project(.gitlab-ci.yml Doxyfile .clang-format)
get_version_info()

if(NOT STATIC_ANALYSIS_ONLY)
	bootstrap_tools()
	get_directory_property(HAS_PARENT PARENT_DIRECTORY)
	if(HAS_PARENT)
		message(STATUS "Providing BOOTSTRAP_LOAD_UNIT_GEN and BOOTSTRAP_PACK_FILE_GEN to parent directory.")
		set(BOOTSTRAP_LOAD_UNIT_GEN ${BOOTSTRAP_LOAD_UNIT_GEN} PARENT_SCOPE)
		set(BOOTSTRAP_PACK_FILE_GEN ${BOOTSTRAP_PACK_FILE_GEN} PARENT_SCOPE)
	else()
		message(STATUS "Using bootstrap tools only internally because there is no parent directory.")
	endif()
	add_subdirectory(multicore_engine_assets)
	add_subdirectory(multicore_engine_test_assets)
endif()

add_subdirectory(multicore_engine_parsers)
add_subdirectory(multicore_engine_base)
add_subdirectory(multicore_engine_core)
add_subdirectory(multicore_engine_renderer)
add_subdirectory(multicore_engine_tests)
add_subdirectory(multicore_engine_graphics_test)
add_subdirectory(multicore_engine_load_unit_gen)
add_subdirectory(multicore_engine_pack_file_gen)
add_subdirectory(multicore_engine_model_converter)

