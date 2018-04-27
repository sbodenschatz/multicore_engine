cmake_minimum_required (VERSION 3.10)
project(multicore_engine_load_unit_gen LANGUAGES CXX)
file(GLOB_RECURSE LOAD_UNIT_GEN_SRC "src/*.cpp")
file(GLOB_RECURSE LOAD_UNIT_GEN_HEADERS "include/*.hpp")
add_executable(mce_load_unit_gen ${LOAD_UNIT_GEN_SRC} ${LOAD_UNIT_GEN_HEADERS})
include(SourceGroupGenerator)
make_src_groups_code("include/mce" "src")
target_link_libraries(mce_load_unit_gen 
		mce_base
		Boost::program_options
	)
enable_custom_lto(mce_load_unit_gen)
