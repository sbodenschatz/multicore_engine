cmake_minimum_required (VERSION 3.10)
project(multicore_engine_base LANGUAGES CXX)
configure_file(src/core/version.cpp.in src/core/version.cpp)
file(GLOB_RECURSE BASE_SRC "src/*.cpp")
file(GLOB_RECURSE BASE_HEADERS "include/*.hpp")
add_library(mce_base STATIC ${BASE_SRC} ${BASE_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/src/core/version.cpp src/core/version.cpp.in)
include(SourceGroupGenerator)
make_src_groups_code("include/mce" "src")
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
set_target_properties(mce_base PROPERTIES EXPORT_NAME base)
enable_custom_lto(mce_base)

install(
		TARGETS mce_base 
		EXPORT mce-dev
		DESTINATION lib
		COMPONENT mce-dev
	)

include(ProvideEmbeddedImport)
provide_embedded_import(mce:: mce_base)

install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
		DESTINATION include
		COMPONENT mce-dev
	)
