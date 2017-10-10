cmake_minimum_required (VERSION 3.7)
project(multicore_engine_headed)
file(GLOB_RECURSE HEADED_SRC "src/*.cpp")
file(GLOB_RECURSE HEADED_HEADERS "include/*.hpp")
add_library(mce_headed STATIC ${HEADED_SRC} ${HEADED_HEADERS})
make_src_groups()
target_include_directories(mce_headed PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(mce_headed 
		mce_parsers 
		mce_base
		mce_core
		deps.glfw
		deps.TBB
		deps.Boost.ASIO
	)
if(MSVC)
	set_target_properties(mce_headed PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_headed PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_headed PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()