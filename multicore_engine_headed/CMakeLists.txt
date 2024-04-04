cmake_minimum_required (VERSION 3.10)
cmake_policy(VERSION 3.10...3.29)

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
		mce_base
		mce_core
		glfw
		TBB::tbb
		Boost::asio
	)
set_target_properties(mce_headed PROPERTIES EXPORT_NAME headed)
enable_custom_lto(mce_headed)

install(
		TARGETS mce_headed 
		EXPORT mce-dev
		DESTINATION lib
		COMPONENT mce-dev
	)

include(ProvideEmbeddedImport)
provide_embedded_import(mce:: mce_headed)

install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
		DESTINATION include
		COMPONENT mce-dev
	)
