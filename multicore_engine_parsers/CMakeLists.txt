cmake_minimum_required (VERSION 3.7)
project(multicore_engine_parsers)
file(GLOB_RECURSE PARSERS_SRC "src/*.cpp")
file(GLOB_RECURSE PARSERS_HEADERS "src/*.hpp")
add_library(multicore_engine_parsers STATIC ${PARSERS_SRC} ${PARSERS_HEADERS})
target_include_directories(multicore_engine_parsers PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
		PRIVATE
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/multicore_engine_base/include>
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/multicore_engine_core/include>
	)
target_link_libraries(multicore_engine_parsers 
		multicore_engine_compiler_settings 
		glm
	)
make_src_groups()
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
	if(WIN32)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
	endif()
endif()
if(MSVC)
	set_target_properties(multicore_engine_parsers PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(multicore_engine_parsers PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(multicore_engine_parsers PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
