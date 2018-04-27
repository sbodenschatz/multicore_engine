include_guard()

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

