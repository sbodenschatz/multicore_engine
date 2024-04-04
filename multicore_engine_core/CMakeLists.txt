cmake_minimum_required (VERSION 3.10)
cmake_policy(VERSION 3.10...3.29)

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
		mce_base
		TBB::tbb
		Boost::asio
		Threads::Threads
	)
set_target_properties(mce_core PROPERTIES EXPORT_NAME core)
enable_custom_lto(mce_core)

install(
		TARGETS mce_core 
		EXPORT mce-dev
		DESTINATION lib
		COMPONENT mce-dev
	)

include(ProvideEmbeddedImport)
provide_embedded_import(mce:: mce_core)

install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
		DESTINATION include
		COMPONENT mce-dev
	)
