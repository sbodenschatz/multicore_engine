cmake_minimum_required (VERSION 3.7)
project(multicore_engine_graphics_test)

make_src_grp_dir()

file(GLOB src_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "assets/*" "assets/*/*" "assets/*/*/*" "assets/*/*/*/*")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets)
foreach(src_dir ${src_dirs})
	if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${src_dir})
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${src_dir})
	endif()
endforeach()

file(GLOB_RECURSE SHADERS_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.vert" "*.tesc" "*.tese" "*.geom" "*.frag" "*.comp")
compile_glsl_to_spirv(SPV_OBJ ${SHADERS_SRC})

file(GLOB_RECURSE LOAD_UNIT_DESCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.lud")
build_load_units(assets LOAD_UNIT_METAS LOAD_UNIT_PAYLOADS ${LOAD_UNIT_DESCS})

file(GLOB_RECURSE PACK_DESCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.packdesc")
build_pack_files(assets PACK_FILES ${PACK_DESCS})

file(GLOB_RECURSE MODEL_OBJS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.obj")
convert_models(MODEL_FILES MODEL_COLS ${MODEL_OBJS})

add_custom_target(mce_graphics_test_compile ALL SOURCES ${SHADERS_SRC} DEPENDS ${SPV_OBJ})
add_custom_target(mce_graphics_test_models ALL SOURCES ${MODEL_OBJS} DEPENDS ${MODEL_FILES} ${MODEL_COLS})
add_custom_target(mce_graphics_test_units ALL SOURCES ${LOAD_UNIT_DESCS} DEPENDS ${LOAD_UNIT_METAS} ${LOAD_UNIT_PAYLOADS})
add_custom_target(mce_graphics_test_packs ALL SOURCES ${PACK_DESCS} DEPENDS ${PACK_FILES})
add_dependencies(mce_graphics_test_units mce_graphics_test_compile mce_graphics_test_models)
add_dependencies(mce_graphics_test_packs mce_graphics_test_units)

file(GLOB_RECURSE GRAPHICS_TEST_SRC "src/*.cpp")
file(GLOB_RECURSE GRAPHICS_TEST_HEADERS "include/*.hpp")
add_executable(mce_graphics_test ${GRAPHICS_TEST_SRC} ${GRAPHICS_TEST_HEADERS})
target_include_directories(mce_graphics_test PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	)
target_link_libraries(mce_graphics_test 
		mce_core
		mce_renderer
		deps.Boost.ProgramOptions
	)
if(MSVC)
	set_target_properties(mce_graphics_test PROPERTIES LINK_FLAGS_RELEASE "/LTCG")
	set_target_properties(mce_graphics_test PROPERTIES LINK_FLAGS_MINSIZEREL "/LTCG")
	set_target_properties(mce_graphics_test PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/LTCG")
endif()
if(NOT STATIC_ANALYSIS_ONLY)
	add_dependencies(mce_graphics_test mce_graphics_test_packs)
endif()
tbb_shared_libs_copy(mce_graphics_test)
