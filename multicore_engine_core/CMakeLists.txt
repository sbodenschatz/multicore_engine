cmake_minimum_required (VERSION 3.7)
project(multicore_engine_core)
file(GLOB_RECURSE CORE_SRC "src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "include/*.hpp")
add_library(mce_core STATIC ${CORE_SRC} ${CORE_HEADERS})
make_src_groups()
if(USE_BLOCKED_COMPONENT_POOLS)
	target_compile_definitions(mce_core PUBLIC MCE_USE_BLOCKED_COMPONENT_POOLS)
endif()
target_include_directories(mce_core PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(mce_core 
		mce_parsers 
		mce_base
		deps.glfw
		deps.TBB
		deps.Boost.ASIO
	)
if(MSVC)
	set_target_properties(mce_core PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_core PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_core PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
