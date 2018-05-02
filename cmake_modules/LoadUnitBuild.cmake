include_guard()

include("${CMAKE_CURRENT_LIST_DIR}/../cmake_internal/BootstrapTools.cmake")

function(build_load_units OUTPUT_META_LIST OUTPUT_PAYLOAD_LIST)
	ensure_tools_bootstrapped()
	set(LUP_LIST)
	set(LUM_LIST)
	foreach(DESC_FILE ${ARGN})
		get_filename_component(BASE_NAME ${DESC_FILE} NAME_WE)
		get_filename_component(DIR ${DESC_FILE} DIRECTORY)
		if(DIR)
			set(PL_FILE ${DIR}/${BASE_NAME}.lup)
			set(META_FILE ${DIR}/${BASE_NAME}.lum)
		else()
			set(PL_FILE ${BASE_NAME}.lup)
			set(META_FILE ${BASE_NAME}.lum)
		endif()
		set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
		execute_process(COMMAND ${BOOTSTRAP_LOAD_UNIT_GEN} --deps -d ${CMAKE_CURRENT_SOURCE_DIR}/${DESC_FILE} WORKING_DIRECTORY ${BUILD_DIR} OUTPUT_VARIABLE DEPS_LIST RESULT_VARIABLE DEPS_RES )
		add_custom_command(
			OUTPUT ${META_FILE} ${PL_FILE}
			COMMAND mce_load_unit_gen -m ${BUILD_DIR}/${META_FILE} -p ${BUILD_DIR}/${PL_FILE} -d ${CMAKE_CURRENT_SOURCE_DIR}/${DESC_FILE}
			MAIN_DEPENDENCY ${DESC_FILE}
			DEPENDS ${DEPS_LIST} mce_load_unit_gen
			WORKING_DIRECTORY ${BUILD_DIR}
			COMMENT "Building load unit from \"${DESC_FILE}\" to meta file \"${META_FILE}\" and payload file \"${PL_FILE}\""
		)
		list(APPEND LUM_LIST ${BUILD_DIR}/${META_FILE})
		list(APPEND LUP_LIST ${BUILD_DIR}/${PL_FILE})
	endforeach()
	set(${OUTPUT_META_LIST} ${LUM_LIST} PARENT_SCOPE)
	set(${OUTPUT_PAYLOAD_LIST} ${LUP_LIST} PARENT_SCOPE)
endfunction()

