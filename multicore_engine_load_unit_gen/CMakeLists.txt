cmake_minimum_required (VERSION 3.7)
project(multicore_engine_load_unit_gen)
file(GLOB_RECURSE LOAD_UNIT_GEN_SRC "src/*.cpp")
file(GLOB_RECURSE LOAD_UNIT_GEN_HEADERS "include/*.hpp")
add_executable(mce_load_unit_gen ${LOAD_UNIT_GEN_SRC} ${LOAD_UNIT_GEN_HEADERS})
make_src_groups()
target_link_libraries(mce_load_unit_gen 
		mce_base
		Boost::program_options
	)
if(MSVC)
	set_target_properties(mce_load_unit_gen PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_load_unit_gen PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_load_unit_gen PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
