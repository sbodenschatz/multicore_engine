cmake_minimum_required (VERSION 3.10)
project(multicore_engine_assets)
include(SourceGroupGenerator)
make_src_groups_dirs()
include(AssetBuildTools)

file(GLOB src_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*" "*/*" "*/*/*" "*/*/*/*")
foreach(src_dir ${src_dirs})
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${src_dir})
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${src_dir})
	endif()
endforeach()

file(GLOB_RECURSE SHADERS_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.vert" "*.tesc" "*.tese" "*.geom" "*.frag" "*.comp")
compile_glsl_to_spirv(SPV_OBJ ${SHADERS_SRC})

file(GLOB_RECURSE LOAD_UNIT_DESCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.lud")
build_load_units(LOAD_UNIT_METAS LOAD_UNIT_PAYLOADS ${LOAD_UNIT_DESCS})

file(GLOB_RECURSE PACK_DESCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.packdesc")
build_pack_files(PACK_FILES ${PACK_DESCS})

file(GLOB_RECURSE MODEL_OBJS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.obj")
convert_models(MODEL_FILES MODEL_COLS ${MODEL_OBJS})

add_custom_target(mce_assets_compile ALL SOURCES ${SHADERS_SRC} DEPENDS ${SPV_OBJ})
add_custom_target(mce_assets_models ALL SOURCES ${MODEL_OBJS} DEPENDS ${MODEL_FILES} ${MODEL_COLS})
add_custom_target(mce_assets_units ALL SOURCES ${LOAD_UNIT_DESCS} DEPENDS ${LOAD_UNIT_METAS} ${LOAD_UNIT_PAYLOADS})
add_custom_target(mce_assets_packs ALL SOURCES ${PACK_DESCS} DEPENDS ${PACK_FILES})
add_dependencies(mce_assets_units mce_assets_compile mce_assets_models)
add_dependencies(mce_assets_packs mce_assets_units)

message(STATUS "PACK_FILES: ${PACK_FILES}")
install(
		FILES ${PACK_FILES}
		DESTINATION assets
		COMPONENT mce
	)
