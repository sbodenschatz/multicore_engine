cmake_minimum_required (VERSION 3.7)
project(multicore_engine_base)
configure_file(src/core/version.cpp.in src/core/version.cpp)
file(GLOB_RECURSE BASE_SRC "src/*.cpp")
file(GLOB_RECURSE BASE_HEADERS "include/*.hpp")
add_library(multicore_engine_base STATIC ${BASE_SRC} ${BASE_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/src/core/version.cpp src/core/version.cpp.in)
make_src_groups()
target_include_directories(multicore_engine_base PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(multicore_engine_base 
		multicore_engine_compiler_settings 
		multicore_engine_parsers
		deps.glm
		deps.zlib
	)
if(MSVC)
	set_target_properties(multicore_engine_base PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(multicore_engine_base PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(multicore_engine_base PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
if(WIN32 AND NOT MSVC)
	target_link_libraries(multicore_engine_base ws2_32)
	target_link_libraries(multicore_engine_base mswsock)
endif()
if(NOT MSVC)
	target_link_libraries(multicore_engine_base boost_program_options.a)
	target_link_libraries(multicore_engine_base boost_filesystem.a)
	target_link_libraries(multicore_engine_base boost_system.a)
	target_link_libraries(multicore_engine_base boost_thread.a)
endif()

