cmake_minimum_required (VERSION 3.10)
project(multicore_engine_headed LANGUAGES CXX)
file(GLOB_RECURSE HEADED_SRC "src/*.cpp")
file(GLOB_RECURSE HEADED_HEADERS "include/*.hpp")
add_library(mce_headed STATIC ${HEADED_SRC} ${HEADED_HEADERS})
include(SourceGroupGenerator)
make_src_groups_code("include/mce" "src")
target_include_directories(mce_headed PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(mce_headed 
		mce_parsers 
		mce_base
		mce_core
		glfw
		TBB::tbb
		Boost::asio
	)
enable_custom_lto(mce_headed)
