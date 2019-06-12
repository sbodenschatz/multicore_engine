include_guard()

include(ProcessorCount)
ProcessorCount(CPU_COUNT)

define_property(GLOBAL 
		PROPERTY MCE_TOOLS_BOOTSTRAP_DONE
		BRIEF_DOCS "Internal global property to detect if the asset build tools of mce were already bootstrapped."
		FULL_DOCS 
			"Internal global property to detect if the asset build tools of mce were already bootstrapped."
			"The tools must be bootstrapped to use them to detect dependencies for the asset build process during CMake configuration."
			"The bootstrap process is started layzily."
			"This property is set when the bootstrap was already done in this CMake run."
	)

function(print_list LIST_NAME)
	foreach(ENTRY ${ARGN})
		message(STATUS "${LIST_NAME}: " ${ENTRY})
	endforeach()
endfunction()

function(bootstrap_tools)
	list(APPEND PASS_TO_BOOTSTRAP 
			LIBS_DIR
			GTEST_ROOT
			BOOST_ROOT
		)

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
		execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" -A "${CMAKE_GENERATOR_PLATFORM}" ${CMAKE_CURRENT_LIST_DIR}/../tools_bootstrap ${BOOTSTRAP_VAR_CMD} WORKING_DIRECTORY tools_bootstrap)
		execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release WORKING_DIRECTORY tools_bootstrap)
		set(BOOTSTRAP_LOAD_UNIT_GEN
			${CMAKE_BINARY_DIR}/tools_bootstrap/multicore_engine_load_unit_gen/Release/mce_load_unit_gen
			CACHE FILEPATH
			"Path of the (bootstrapped) load unit generator binary used to determine dependencies during CMake configuration."
		)
		set(BOOTSTRAP_PACK_FILE_GEN
			${CMAKE_BINARY_DIR}/tools_bootstrap/multicore_engine_pack_file_gen/Release/mce_pack_file_gen
			CACHE FILEPATH
			"Path of the (bootstrapped) pack file generator binary used to determine dependencies during CMake configuration."
		)
		mark_as_advanced(BOOTSTRAP_LOAD_UNIT_GEN BOOTSTRAP_PACK_FILE_GEN)
	else()
		file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tools_bootstrap)
		execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_LIST_DIR}/../tools_bootstrap -DCMAKE_BUILD_TYPE=Release ${BOOTSTRAP_VAR_CMD} WORKING_DIRECTORY tools_bootstrap)
		execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release -- -j ${CPU_COUNT} WORKING_DIRECTORY tools_bootstrap)
		set(BOOTSTRAP_LOAD_UNIT_GEN
			${CMAKE_BINARY_DIR}/tools_bootstrap/multicore_engine_load_unit_gen/mce_load_unit_gen
			CACHE FILEPATH
			"Path of the (bootstrapped) load unit generator binary used to determine dependencies during CMake configuration."
		)
		set(BOOTSTRAP_PACK_FILE_GEN
			${CMAKE_BINARY_DIR}/tools_bootstrap/multicore_engine_pack_file_gen/mce_pack_file_gen
			CACHE FILEPATH
			"Path of the (bootstrapped) pack file generator binary used to determine dependencies during CMake configuration."
		)
		mark_as_advanced(BOOTSTRAP_LOAD_UNIT_GEN BOOTSTRAP_PACK_FILE_GEN)
	endif()
	MESSAGE(STATUS "Bootstrapped asset tools.")
endfunction()

function(ensure_tools_bootstrapped)
	get_property(bootstrap_done GLOBAL PROPERTY MCE_TOOLS_BOOTSTRAP_DONE SET)
	if(NOT bootstrap_done)
		bootstrap_tools()
		set_property(GLOBAL PROPERTY MCE_TOOLS_BOOTSTRAP_DONE 1)
	endif()
	if(NOT DEFINED BOOTSTRAP_LOAD_UNIT_GEN)
		message(WARNING "BOOTSTRAP_LOAD_UNIT_GEN not set after triggering bootstrap.")
	endif()
	if(NOT DEFINED BOOTSTRAP_PACK_FILE_GEN)
		message(WARNING "BOOTSTRAP_PACK_FILE_GEN not set after triggering bootstrap.")
	endif()
endfunction()
