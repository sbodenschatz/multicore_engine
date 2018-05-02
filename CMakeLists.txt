cmake_minimum_required (VERSION 3.10)
# Language C is needed for phthreads detection to work correctly
project (multicore_engine LANGUAGES CXX C)

include("${CMAKE_CURRENT_LIST_DIR}/UseModules.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_internal")

enable_testing()

include(BuildConfig)
include(IntegrateBuildScripts)
include(RepositoryVersionDetector)

make_build_scripts_project(MCE .gitlab-ci.yml .gitlab-ci-edges-env.yml Doxyfile .clang-format create_license_pack_linux.sh create_license_pack_windows.bat)
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

install(
		EXPORT mce-dev 
		DESTINATION cmake/mce
		NAMESPACE mce::
		COMPONENT mce-dev
		FILE mceConfig.cmake
	)

install(
		DIRECTORY
			${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules
			${CMAKE_CURRENT_SOURCE_DIR}/cmake_helpers
			${CMAKE_CURRENT_SOURCE_DIR}/cmake_internal
		DESTINATION cmake/mce
		COMPONENT mce-dev
	)

install(
		FILES ${CMAKE_CURRENT_SOURCE_DIR}/UseModules.cmake
		DESTINATION cmake/mce
		COMPONENT mce-dev
	)

export(
		EXPORT mce-dev 
		NAMESPACE mce::
		FILE mceConfig.cmake
	)

include(ComponentInstallTargets)
define_component_install_targets(mce mce-dev mce-test)
