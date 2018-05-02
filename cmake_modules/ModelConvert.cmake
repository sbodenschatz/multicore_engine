include_guard()

function(convert_models OUTPUT_MODEL_LIST OUTPUT_COL_LIST)
	set(MODEL_LIST)
	set(COL_LIST)
	foreach(SRC_FILE ${ARGN})
		get_filename_component(BASE_NAME ${SRC_FILE} NAME_WE)
		get_filename_component(DIR ${SRC_FILE} DIRECTORY)
		if(DIR)
			set(MODEL_FILE ${DIR}/${BASE_NAME}.model)
			set(COL_FILE ${DIR}/${BASE_NAME}.col)
		else()
			set(MODEL_FILE ${BASE_NAME}.model)
			set(COL_FILE ${BASE_NAME}.col)
		endif()
		set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
		add_custom_command(
			OUTPUT ${MODEL_FILE} ${COL_FILE}
			COMMAND mce_model_converter -m ${BUILD_DIR}/${MODEL_FILE} -c ${BUILD_DIR}/${COL_FILE} -i ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_FILE}
			MAIN_DEPENDENCY ${SRC_FILE}
			DEPENDS mce_model_converter
			WORKING_DIRECTORY ${BUILD_DIR}
			COMMENT "Converting model input file \"${SRC_FILE}\" to model file \"${MODEL_FILE}\" and collision file \"${COL_FILE}\""
		)
		list(APPEND MODEL_LIST ${BUILD_DIR}/${MODEL_FILE})
		list(APPEND COL_LIST ${BUILD_DIR}/${COL_FILE})
	endforeach()
	set(${OUTPUT_MODEL_LIST} ${MODEL_LIST} PARENT_SCOPE)
	set(${OUTPUT_COL_LIST} ${COL_LIST} PARENT_SCOPE)
endfunction()

