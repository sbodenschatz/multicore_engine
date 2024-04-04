cmake_minimum_required (VERSION 3.10)
cmake_policy(VERSION 3.10...3.29)

file(GLOB_RECURSE PACK_FILE_GEN_SRC "src/*.cpp")
file(GLOB_RECURSE PACK_FILE_GEN_HEADERS "include/*.hpp")
add_executable(mce_pack_file_gen ${PACK_FILE_GEN_SRC} ${PACK_FILE_GEN_HEADERS})
include(SourceGroupGenerator)
make_src_groups_code("include/mce" "src")
target_link_libraries(mce_pack_file_gen 
		mce_base
		Boost::program_options
	)
set_target_properties(mce_pack_file_gen PROPERTIES EXPORT_NAME pack_file_gen)
enable_custom_lto(mce_pack_file_gen)

install(
		TARGETS mce_pack_file_gen
		DESTINATION bin
		COMPONENT mce
	)
