cmake_minimum_required (VERSION 3.7)
project(multicore_engine_graphics_test)
file(GLOB_RECURSE GRAPHICS_TEST_SRC "src/*.cpp")
file(GLOB_RECURSE GRAPHICS_TEST_HEADERS "include/*.hpp")
add_executable(mce_graphics_test ${GRAPHICS_TEST_SRC} ${GRAPHICS_TEST_HEADERS})
target_include_directories(mce_graphics_test PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(mce_graphics_test 
		mce_core
		mce_renderer
		deps.Boost.ProgramOptions
	)
if(MSVC)
	set_target_properties(mce_graphics_test PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_graphics_test PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_graphics_test PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
if(NOT STATIC_ANALYSIS_ONLY)
	add_dependencies(mce_graphics_test mce_assets_packs)
endif()
