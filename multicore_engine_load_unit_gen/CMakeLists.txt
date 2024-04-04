cmake_minimum_required (VERSION 3.10)
cmake_policy(VERSION 3.10...3.29)

file(GLOB_RECURSE LOAD_UNIT_GEN_SRC "src/*.cpp")
file(GLOB_RECURSE LOAD_UNIT_GEN_HEADERS "include/*.hpp")
add_executable(mce_load_unit_gen ${LOAD_UNIT_GEN_SRC} ${LOAD_UNIT_GEN_HEADERS})
include(SourceGroupGenerator)
make_src_groups_code("include/mce" "src")
target_link_libraries(mce_load_unit_gen 
		mce_base
		Boost::program_options
	)
set_target_properties(mce_load_unit_gen PROPERTIES EXPORT_NAME load_unit_gen)
enable_custom_lto(mce_load_unit_gen)

install(
		TARGETS mce_load_unit_gen
		DESTINATION bin
		COMPONENT mce
	)
