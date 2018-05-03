cmake_minimum_required (VERSION 3.10)
project(multicore_engine_graphics_test LANGUAGES CXX)
file(GLOB_RECURSE GRAPHICS_TEST_SRC "src/*.cpp")
file(GLOB_RECURSE GRAPHICS_TEST_HEADERS "include/*.hpp")
add_executable(mce_graphics_test ${GRAPHICS_TEST_SRC} ${GRAPHICS_TEST_HEADERS})
target_include_directories(mce_graphics_test PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(mce_graphics_test 
		mce_core
		mce_headed
		mce_renderer
		Boost::program_options
	)
set_target_properties(mce_graphics_test PROPERTIES EXPORT_NAME graphics_test)
enable_custom_lto(mce_graphics_test)
if(NOT STATIC_ANALYSIS_ONLY)
	add_dependencies(mce_graphics_test mce_test_assets_packs)
endif()
include(SharedLibsCopy)
shared_libs_copy(mce_graphics_test SHARED_LIBS TBB::tbb TBB::tbbmalloc)

install(
		TARGETS mce_graphics_test
		DESTINATION bin
		COMPONENT mce-test
	)

install(
		FILES ${SHARED_LIBS}
		DESTINATION bin
		COMPONENT mce-test
	)
