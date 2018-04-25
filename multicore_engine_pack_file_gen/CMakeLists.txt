cmake_minimum_required (VERSION 3.10)
project(multicore_engine_pack_file_gen)
file(GLOB_RECURSE PACK_FILE_GEN_SRC "src/*.cpp")
file(GLOB_RECURSE PACK_FILE_GEN_HEADERS "include/*.hpp")
add_executable(mce_pack_file_gen ${PACK_FILE_GEN_SRC} ${PACK_FILE_GEN_HEADERS})
make_src_groups()
target_link_libraries(mce_pack_file_gen 
		mce_base
		Boost::program_options
	)
if(MSVC)
	set_target_properties(mce_pack_file_gen PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_pack_file_gen PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_pack_file_gen PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
