cmake_minimum_required (VERSION 3.7)
project(multicore_engine_model_converter)
file(GLOB_RECURSE MODEL_CONV_SRC "src/*.cpp")
file(GLOB_RECURSE MODEL_CONV_HEADERS "include/*.hpp")
add_executable(mce_model_converter ${MODEL_CONV_SRC} ${MODEL_CONV_HEADERS})
make_src_groups()
target_link_libraries(mce_model_converter 
		mce_base
		Boost::program_options
	)
if(MSVC)
	set_target_properties(mce_model_converter PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_model_converter PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_model_converter PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
