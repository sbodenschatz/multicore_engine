include(ProcessorCount)
ProcessorCount(CPU_COUNT)

include(SourceGroupGenerator)
include(VulkanShaderCompiler)
include(IntegrateBuildScripts)
include(RepositoryVersionDetector)

function(build_load_units OUTPUT_META_LIST OUTPUT_PAYLOAD_LIST)
	set(LUP_LIST)
	set(LUM_LIST)
	foreach(DESC_FILE ${ARGN})
		get_filename_component(BASE_NAME ${DESC_FILE} NAME_WE)
		get_filename_component(DIR ${DESC_FILE} DIRECTORY)
		set(PL_FILE ${DIR}${BASE_NAME}.lup)
		set(META_FILE ${DIR}${BASE_NAME}.lum)
		execute_process(COMMAND ${BOOTSTRAP_LOAD_UNIT_GEN} --deps -d ${CMAKE_CURRENT_SOURCE_DIR}/${DESC_FILE} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} OUTPUT_VARIABLE DEPS_LIST RESULT_VARIABLE DEPS_RES )
		add_custom_command(
			OUTPUT ${META_FILE} ${PL_FILE}
			COMMAND mce_load_unit_gen -m ${META_FILE} -p ${PL_FILE} -d ${CMAKE_CURRENT_SOURCE_DIR}/${DESC_FILE}
			MAIN_DEPENDENCY ${DESC_FILE}
			DEPENDS ${DEPS_LIST} mce_load_unit_gen
			COMMENT "Building load unit from \"${DESC_FILE}\" to meta file \"${META_FILE}\" and payload file \"${PL_FILE}\""
		)
		list(APPEND LUM_LIST ${META_FILE})
		list(APPEND LUP_LIST ${PL_FILE})
	endforeach()
	set(${OUTPUT_META_LIST} ${LUM_LIST} PARENT_SCOPE)
	set(${OUTPUT_PAYLOAD_LIST} ${LUP_LIST} PARENT_SCOPE)
endfunction()

function(print_list LIST_NAME)
	foreach(ENTRY ${ARGN})
		message(STATUS "${LIST_NAME}: " ${ENTRY})
	endforeach()
endfunction()

function(bootstrap_tools)
	MESSAGE(STATUS "Bootstraping asset tools ...")
	set(BOOTSTRAP_VAR_CMD)
	foreach(VARNAME ${PASS_TO_BOOTSTRAP})
		if(DEFINED ${VARNAME})
			list(APPEND BOOTSTRAP_VAR_CMD "-D${VARNAME}=${${VARNAME}}")
		endif()
	endforeach()
	print_list("	Forwarded variables for bootstrap" ${BOOTSTRAP_VAR_CMD})
	if(MSVC)
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tools_bootstrap)
		execute_process(COMMAND cmake -G "${CMAKE_GENERATOR}" ${PROJECT_SOURCE_DIR}/tools_bootstrap ${BOOTSTRAP_VAR_CMD} WORKING_DIRECTORY tools_bootstrap)
		execute_process(COMMAND cmake --build . --config Release WORKING_DIRECTORY tools_bootstrap)
		set(BOOTSTRAP_LOAD_UNIT_GEN ${CMAKE_BINARY_DIR}/tools_bootstrap/multicore_engine_load_unit_gen/Release/mce_load_unit_gen PARENT_SCOPE)
		set(BOOTSTRAP_PACK_FILE_GEN ${CMAKE_BINARY_DIR}/tools_bootstrap/multicore_engine_pack_file_gen/Release/mce_pack_file_gen PARENT_SCOPE)
	else()
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tools_bootstrap)
		execute_process(COMMAND cmake -G "${CMAKE_GENERATOR}" ${PROJECT_SOURCE_DIR}/tools_bootstrap -DCMAKE_BUILD_TYPE=Release ${BOOTSTRAP_VAR_CMD} WORKING_DIRECTORY tools_bootstrap)
		execute_process(COMMAND cmake --build . --config Release -- -j ${CPU_COUNT} WORKING_DIRECTORY tools_bootstrap)
		set(BOOTSTRAP_LOAD_UNIT_GEN ${CMAKE_BINARY_DIR}/tools_bootstrap/multicore_engine_load_unit_gen/mce_load_unit_gen PARENT_SCOPE)
		set(BOOTSTRAP_PACK_FILE_GEN ${CMAKE_BINARY_DIR}/tools_bootstrap/multicore_engine_pack_file_gen/mce_pack_file_gen PARENT_SCOPE)
	endif()
	MESSAGE(STATUS "Bootstrapped asset tools.")
endfunction()

function(build_pack_files OUTPUT_LIST)
	set(PACK_LIST)
	foreach(DESC_FILE ${ARGN})
		get_filename_component(BASE_NAME ${DESC_FILE} NAME_WE)
		get_filename_component(DIR ${DESC_FILE} DIRECTORY)
		set(PACK_FILE ${DIR}${BASE_NAME}.pack)
		execute_process(COMMAND ${BOOTSTRAP_PACK_FILE_GEN} --deps -d ${CMAKE_CURRENT_SOURCE_DIR}/${DESC_FILE} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} OUTPUT_VARIABLE DEPS_LIST RESULT_VARIABLE DEPS_RES)
		add_custom_command(
			OUTPUT ${PACK_FILE}
			COMMAND mce_pack_file_gen -o ${PACK_FILE} -d ${CMAKE_CURRENT_SOURCE_DIR}/${DESC_FILE}
			MAIN_DEPENDENCY ${DESC_FILE}
			DEPENDS ${DEPS_LIST} mce_pack_file_gen
			COMMENT "Building pack file \"${PACK_FILE}\" from \"${DESC_FILE}\""
		)
		list(APPEND PACK_LIST ${PACK_FILE})
	endforeach()
	set(${OUTPUT_LIST} ${PACK_LIST} PARENT_SCOPE)
endfunction()

function(convert_models OUTPUT_MODEL_LIST OUTPUT_COL_LIST)
	set(MODEL_LIST)
	set(COL_LIST)
	foreach(SRC_FILE ${ARGN})
		get_filename_component(BASE_NAME ${SRC_FILE} NAME_WE)
		get_filename_component(DIR ${SRC_FILE} DIRECTORY)
		set(MODEL_FILE ${DIR}/${BASE_NAME}.model)
		set(COL_FILE ${DIR}/${BASE_NAME}.col)
		add_custom_command(
			OUTPUT ${MODEL_FILE} ${COL_FILE}
			COMMAND mce_model_converter -m ${MODEL_FILE} -c ${COL_FILE} -i ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_FILE}
			MAIN_DEPENDENCY ${SRC_FILE}
			DEPENDS mce_model_converter
			COMMENT "Converting model input file \"${SRC_FILE}\" to model file \"${MODEL_FILE}\" and collision file \"${COL_FILE}\""
		)
		list(APPEND MODEL_LIST ${MODEL_FILE})
		list(APPEND COL_LIST ${COL_FILE})
	endforeach()
	set(${OUTPUT_MODEL_LIST} ${MODEL_LIST} PARENT_SCOPE)
	set(${OUTPUT_COL_LIST} ${COL_LIST} PARENT_SCOPE)
endfunction()

function(shared_libs_copy EXE_TARGET)
	if(WIN32)
		set(file_list)
		foreach(LIB_TARGET ${ARGN})
			get_target_property(LOC_DEBUG ${LIB_TARGET} IMPORTED_LOCATION_DEBUG)
			get_target_property(LOC_RELEASE ${LIB_TARGET} IMPORTED_LOCATION_RELEASE)
			list(APPEND file_list 
				$<$<CONFIG:DEBUG>:"${LOC_DEBUG}">
				$<$<CONFIG:RELWITHDEBINFO>:"${LOC_RELEASE}">
				$<$<CONFIG:MINSIZEREL>:"${LOC_RELEASE}">
				$<$<CONFIG:RELEASE>:"${LOC_RELEASE}">
			)
		endforeach()
		add_custom_command(
				TARGET ${EXE_TARGET} POST_BUILD 
				COMMAND ${CMAKE_COMMAND} -E copy_if_different 
				${file_list}
				$<TARGET_FILE_DIR:${EXE_TARGET}>
			)
	endif()
endfunction()

macro(detect_compiler_name)
	if(MSVC)
		set(compiler_name "msvc")
	elseif(MINGW)
		set(compiler_name "mingw")
	elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
		set(compiler_name "gcc")
	elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
		set(compiler_name "clang")
	else()
		set(compiler_name "unknown")
	endif()
endmacro()