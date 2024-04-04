cmake_minimum_required (VERSION 3.10)
cmake_policy(VERSION 3.10...3.29)

include("${CMAKE_CURRENT_LIST_DIR}/UseModules.cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_internal")

include(RepositoryVersionDetector)
get_version_info()

project (multicore_engine
		VERSION ${BUILD_VERSION_MAJOR}.${BUILD_VERSION_MINOR}.${BUILD_VERSION_PATCH}.${BUILD_VERSION_TWEAK} 
		# Language C is needed for pthreads detection to work correctly
		LANGUAGES CXX C
	)

include(IntegrateBuildScripts)
make_build_scripts_project(MCE .gitlab-ci.yml .gitlab-ci-edges-env.yml Doxyfile .clang-format create_license_pack_linux.sh create_license_pack_windows.bat)

enable_testing()

include(BuildConfig)

add_subdirectory(multicore_engine_base)
add_subdirectory(multicore_engine_core)
add_subdirectory(multicore_engine_headed)
add_subdirectory(multicore_engine_renderer)
add_subdirectory(multicore_engine_tests)
add_subdirectory(multicore_engine_graphics_test)
add_subdirectory(multicore_engine_load_unit_gen)
add_subdirectory(multicore_engine_pack_file_gen)
add_subdirectory(multicore_engine_model_converter)

if(NOT STATIC_ANALYSIS_ONLY)
	add_subdirectory(multicore_engine_assets)
	add_subdirectory(multicore_engine_test_assets)
endif()

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

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/mceConfigVersion.cmake
		VERSION ${multicore_engine_VERSION}
		COMPATIBILITY SameMajorVersion
	)

install(
		FILES
			${CMAKE_CURRENT_SOURCE_DIR}/UseModules.cmake
			${CMAKE_CURRENT_BINARY_DIR}/mceConfigVersion.cmake
		DESTINATION cmake/mce
		COMPONENT mce-dev
	)

install(
		FILES ${CMAKE_CURRENT_SOURCE_DIR}/default.cfg
		DESTINATION .
		COMPONENT mce
	)

export(
		EXPORT mce-dev 
		NAMESPACE mce::
		FILE mceConfig.cmake
	)

include(ComponentInstallTargets)
define_component_install_targets(mce mce-dev mce-test)
