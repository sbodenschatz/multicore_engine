cmake_minimum_required (VERSION 2.6)
project(multicore_engine_assets)
make_src_grp_dir()

file(GLOB src_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*" "*/*" "*/*/*" "*/*/*/*")
foreach(src_dir ${src_dirs})
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${src_dir})
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${src_dir})
	endif()
endforeach()

file(GLOB_RECURSE SHADERS_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.vert" "*.tesc" "*.tese" "*.geom" "*.frag" "*.comp")
compile_glsl_to_spirv(SPV_OBJ ${SHADERS_SRC})

add_custom_target(multicore_engine_assets ALL SOURCES ${SHADERS_SRC} DEPENDS ${SPV_OBJ})
