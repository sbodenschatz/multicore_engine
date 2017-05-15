cmake_minimum_required (VERSION 3.7)
project(multicore_engine_demo)
file(GLOB_RECURSE DEMO_SRC "src/*.cpp")
file(GLOB_RECURSE DEMO_HEADERS "include/*.hpp")
add_executable(mce_demo ${DEMO_SRC} ${DEMO_HEADERS})
target_link_libraries(mce_demo 
		mce_core
		mce_renderer
		deps.Boost.ProgramOptions
	)
if(MSVC)
	set_target_properties(mce_demo PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_demo PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_demo PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
if(NOT STATIC_ANALYSIS_ONLY)
	add_dependencies(mce_demo mce_assets_packs)
endif()
