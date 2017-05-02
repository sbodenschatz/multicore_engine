cmake_minimum_required (VERSION 3.7)
project(multicore_engine_core)
file(GLOB_RECURSE CORE_SRC "src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "include/*.hpp")
add_library(multicore_engine_core STATIC ${CORE_SRC} ${CORE_HEADERS})
make_src_groups()
target_include_directories(multicore_engine_core PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(multicore_engine_core 
		multicore_engine_parsers 
		multicore_engine_base
		deps.vulkan
		deps.glfw
		deps.TBB
		deps.Boost.ASIO
	)
if(MSVC)
	set_target_properties(multicore_engine_core PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(multicore_engine_core PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(multicore_engine_core PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
