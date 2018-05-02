function(shared_libs_copy EXE_TARGET OUTPUT_LIST)
	if(WIN32)
		set(file_list)
		foreach(LIB_TARGET ${ARGN})
			get_target_property(LOC_DEBUG ${LIB_TARGET} IMPORTED_LOCATION_DEBUG)
			get_target_property(LOC_RELEASE ${LIB_TARGET} IMPORTED_LOCATION_RELEASE)
			list(APPEND file_list 
				$<$<CONFIG:DEBUG>:${LOC_DEBUG}>
				$<$<CONFIG:RELWITHDEBINFO>:${LOC_RELEASE}>
				$<$<CONFIG:MINSIZEREL>:${LOC_RELEASE}>
				$<$<CONFIG:RELEASE>:${LOC_RELEASE}>
			)
		endforeach()
		add_custom_command(
				TARGET ${EXE_TARGET} POST_BUILD 
				COMMAND ${CMAKE_COMMAND} -E copy_if_different 
				${file_list}
				$<TARGET_FILE_DIR:${EXE_TARGET}>
			)
	endif()
	set(${OUTPUT_LIST} ${file_list} PARENT_SCOPE)
endfunction()
