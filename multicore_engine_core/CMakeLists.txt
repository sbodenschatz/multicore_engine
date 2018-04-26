cmake_minimum_required (VERSION 3.10)
project(multicore_engine_core)
file(GLOB_RECURSE CORE_SRC "src/*.cpp")
file(GLOB_RECURSE CORE_HEADERS "include/*.hpp")
add_library(mce_core STATIC ${CORE_SRC} ${CORE_HEADERS})
include(SourceGroupGenerator)
make_src_groups_code("include/mce" "src")
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
		TBB::tbb
		Boost::asio
	)
enable_custom_lto(mce_core)
