cmake_minimum_required (VERSION 3.10)
project(multicore_engine_parsers)
file(GLOB_RECURSE PARSERS_SRC "src/*.cpp")
file(GLOB_RECURSE PARSERS_HEADERS "src/*.hpp")
add_library(mce_parsers STATIC ${PARSERS_SRC} ${PARSERS_HEADERS})
target_include_directories(mce_parsers PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
		PRIVATE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../multicore_engine_base/include>
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../multicore_engine_core/include>
	)
target_link_libraries(mce_parsers 
		mce_compiler_settings 
		glm
		Boost::boost
	)
include(SourceGroupGenerator)
make_src_groups_code("include/mce" "src")
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og")
	if(WIN32)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
	endif()
endif()
if(MSVC)
	set_target_properties(mce_parsers PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_parsers PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_parsers PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
