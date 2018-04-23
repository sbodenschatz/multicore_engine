cmake_minimum_required (VERSION 3.7)
project(multicore_engine_base)
configure_file(src/core/version.cpp.in src/core/version.cpp)
file(GLOB_RECURSE BASE_SRC "src/*.cpp")
file(GLOB_RECURSE BASE_HEADERS "include/*.hpp")
add_library(mce_base STATIC ${BASE_SRC} ${BASE_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/src/core/version.cpp src/core/version.cpp.in)
make_src_groups()
target_include_directories(mce_base PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(mce_base 
		mce_compiler_settings 
		mce_parsers
		glm
		ZLIB::ZLIB
		Boost::disable_autolinking
		Boost::boost
		Boost::thread
		Boost::filesystem
	)
if(MSVC)
	set_target_properties(mce_base PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_base PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_base PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
