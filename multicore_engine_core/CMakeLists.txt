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
	)
if(WIN32)
	target_link_libraries(multicore_engine_core debug tbb_debug optimized tbb)
	target_link_libraries(multicore_engine_core debug tbbmalloc_debug optimized tbbmalloc)
elseif(UNIX)
	target_link_libraries(multicore_engine_core tbb)
	target_link_libraries(multicore_engine_core tbbmalloc)
endif()
if(MSVC)
	set_target_properties(multicore_engine_core PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(multicore_engine_core PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(multicore_engine_core PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
if(WIN32 AND NOT MSVC)
	target_link_libraries(multicore_engine_core ws2_32)
	target_link_libraries(multicore_engine_core mswsock)
endif()
if(NOT MSVC)
#	target_link_libraries(multicore_engine_core pthread)
	target_link_libraries(multicore_engine_core boost_program_options.a)
	target_link_libraries(multicore_engine_core boost_filesystem.a)
	target_link_libraries(multicore_engine_core boost_system.a)
	target_link_libraries(multicore_engine_core boost_thread.a)
endif()

