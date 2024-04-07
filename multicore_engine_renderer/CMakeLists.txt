cmake_minimum_required (VERSION 3.10)
cmake_policy(VERSION 3.10...3.29)

file(GLOB_RECURSE RENDERER_SRC "src/*.cpp")
file(GLOB_RECURSE RENDERER_HEADERS "include/*.hpp")
add_library(mce_renderer STATIC ${RENDERER_SRC} ${RENDERER_HEADERS})
include(SourceGroupGenerator)
make_src_groups_code("include/mce" "src")
target_include_directories(mce_renderer PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(mce_renderer 
		mce_core
		mce_headed
		Vulkan::Vulkan
		gli
		vkglformat::vkglformat
	)
set_target_properties(mce_renderer PROPERTIES EXPORT_NAME renderer)
enable_custom_lto(mce_renderer)

install(
		TARGETS mce_renderer 
		EXPORT mce-dev
		DESTINATION lib
		COMPONENT mce-dev
	)

include(ProvideEmbeddedImport)
provide_embedded_import(mce:: mce_renderer)

install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
		DESTINATION include
		COMPONENT mce-dev
	)
