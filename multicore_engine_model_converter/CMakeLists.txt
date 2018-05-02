cmake_minimum_required (VERSION 3.10)
project(multicore_engine_model_converter LANGUAGES CXX)
file(GLOB_RECURSE MODEL_CONV_SRC "src/*.cpp")
file(GLOB_RECURSE MODEL_CONV_HEADERS "include/*.hpp")
add_executable(mce_model_converter ${MODEL_CONV_SRC} ${MODEL_CONV_HEADERS})
include(SourceGroupGenerator)
make_src_groups_code("include/mce" "src")
target_link_libraries(mce_model_converter 
		mce_base
		Boost::program_options
	)
enable_custom_lto(mce_model_converter)

install(
		TARGETS mce_model_converter
		DESTINATION bin
		COMPONENT mce
	)
