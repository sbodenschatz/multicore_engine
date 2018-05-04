include_guard()

add_library(mce_compiler_settings INTERFACE)
target_compile_definitions(mce_compiler_settings INTERFACE
		$<$<CONFIG:Debug>:DEBUG>
		$<$<NOT:$<CONFIG:Debug>>:NDEBUG>
		$<$<PLATFORM_ID:Windows>:MULTICORE_ENGINE_WINDOWS _WIN32_WINNT=0x0601 WINVER=0x0601>
		$<$<NOT:$<PLATFORM_ID:Windows>>:MULTICORE_ENGINE_NOT_WINDOWS>
		$<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING>
		$<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING> #TODO Fix code to take care of the associated warning
	)
target_compile_options(mce_compiler_settings INTERFACE
		$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>: -std=c++1z -Wall -Wextra -Werror -fno-strict-aliasing -Wold-style-cast>
		$<$<CXX_COMPILER_ID:Clang>: -stdlib=libc++ -Wno-unused-private-field>
		$<$<CXX_COMPILER_ID:GNU>:>
		$<$<CXX_COMPILER_ID:MSVC>: /W4 /WX /MP /std:c++latest>
	)
if(NOT DEBUG_O0)
	target_compile_options(mce_compiler_settings INTERFACE
			$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>: -Og>
		)
endif()
target_link_libraries(mce_compiler_settings INTERFACE
		$<$<CXX_COMPILER_ID:Clang>: -stdlib=libc++>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>: atomic>
	)
set_target_properties(mce_compiler_settings PROPERTIES EXPORT_NAME compiler_settings)

include(ProvideEmbeddedImport)
provide_embedded_import(mce:: mce_compiler_settings)

install(TARGETS mce_compiler_settings 
		EXPORT mce-dev
		DESTINATION lib
		COMPONENT mce-dev
	)
