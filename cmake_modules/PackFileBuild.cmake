include_guard()

include("${CMAKE_CURRENT_LIST_DIR}/../cmake_internal/BootstrapTools.cmake")

function(build_pack_files OUTPUT_LIST)
	ensure_tools_bootstrapped()
	set(PACK_LIST)
	foreach(DESC_FILE ${ARGN})
		get_filename_component(BASE_NAME ${DESC_FILE} NAME_WE)
		get_filename_component(DIR ${DESC_FILE} DIRECTORY)
		if(DIR)
			set(PACK_FILE ${DIR}/${BASE_NAME}.pack)
		else()
			set(PACK_FILE ${BASE_NAME}.pack)
		endif()
		set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
		execute_process(COMMAND ${BOOTSTRAP_PACK_FILE_GEN} --deps -d ${CMAKE_CURRENT_SOURCE_DIR}/${DESC_FILE} WORKING_DIRECTORY ${BUILD_DIR} OUTPUT_VARIABLE DEPS_LIST RESULT_VARIABLE DEPS_RES)
		add_custom_command(
			OUTPUT ${PACK_FILE}
			COMMAND mce_pack_file_gen -o ${BUILD_DIR}/${PACK_FILE} -d ${CMAKE_CURRENT_SOURCE_DIR}/${DESC_FILE}
			MAIN_DEPENDENCY ${DESC_FILE}
			DEPENDS ${DEPS_LIST} mce_pack_file_gen
			WORKING_DIRECTORY ${BUILD_DIR}
			COMMENT "Building pack file \"${PACK_FILE}\" from \"${DESC_FILE}\""
		)
		list(APPEND PACK_LIST ${BUILD_DIR}/${PACK_FILE})
	endforeach()
	set(${OUTPUT_LIST} ${PACK_LIST} PARENT_SCOPE)
endfunction()

