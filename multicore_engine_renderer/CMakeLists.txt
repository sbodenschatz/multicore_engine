cmake_minimum_required (VERSION 3.7)
project(multicore_engine_renderer)
file(GLOB_RECURSE RENDERER_SRC "src/*.cpp")
file(GLOB_RECURSE RENDERER_HEADERS "include/*.hpp")
add_library(mce_renderer STATIC ${RENDERER_SRC} ${RENDERER_HEADERS})
make_src_groups()
target_include_directories(mce_renderer PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(mce_renderer 
		mce_core
		mce_headed
		deps.vulkan
		gli
		deps.vkglformat
	)
if(MSVC)
	set_target_properties(mce_renderer PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_renderer PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_renderer PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
