include_guard()

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/BootstrapTools.cmake")

function(build_load_units OUTPUT_META_LIST OUTPUT_PAYLOAD_LIST)
	ensure_tools_bootstrapped()
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

