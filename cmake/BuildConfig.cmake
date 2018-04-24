if(NOT MSVC)
	if(NOT (CMAKE_BUILD_TYPE STREQUAL Debug) AND NOT (CMAKE_BUILD_TYPE STREQUAL Release))
		set(CMAKE_BUILD_TYPE Release)
	endif()
	string(TOLOWER ${CMAKE_BUILD_TYPE} CONFIG_LOWER)
endif()
if(WIN32)
	if(MSVC)
		set(COMPILER_LOWER "vc")
	elseif(MINGW)
		set(COMPILER_LOWER "mingw")
	endif()
elseif(UNIX)
	if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
		set(COMPILER_LOWER "gcc")
	elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
		set(COMPILER_LOWER "clang")
	endif()
endif()

if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

set(GIT_BINARY git CACHE FILEPATH "Git binary to use for version determination.")
set(VERSION "[unknown version]" CACHE STRING "Fallback version to use if auto-detection fails.")
set(BRANCH "[unknown branch]" CACHE STRING "Fallback branch name to use if auto-detection fails.")

if (MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX /ignore:4221 notelemetry.obj")
	set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /WX /ignore:4221 notelemetry.obj")

	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /debug:fastlink /INCREMENTAL")

	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG:incremental /INCREMENTAL:NO /OPT:REF")
	set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /LTCG:incremental")

	set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /LTCG:incremental /INCREMENTAL:NO /OPT:REF")
	set(CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL "${CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL} /LTCG:incremental")

	set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /LTCG:incremental /INCREMENTAL:NO /OPT:REF")
	set(CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO} /LTCG:incremental")
endif ()

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
		#$<$<CXX_COMPILER_ID:Clang>: -stdlib=libc++ -Wno-unused-private-field>
		$<$<CXX_COMPILER_ID:Clang>: -Wno-unused-private-field>
		$<$<CXX_COMPILER_ID:GNU>:>
		$<$<CXX_COMPILER_ID:MSVC>: /W4 /WX /MP /std:c++latest>
		$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<CONFIG:DEBUG>>>: /GL>
	)
if(NOT DEBUG_O0)
	target_compile_options(mce_compiler_settings INTERFACE
			$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>: -Og>
		)
endif()
target_link_libraries(mce_compiler_settings INTERFACE
		#$<$<CXX_COMPILER_ID:Clang>: -stdlib=libc++>
		$<$<NOT:$<CXX_COMPILER_ID:MSVC>>: pthread atomic>
	)
target_include_directories(mce_compiler_settings SYSTEM INTERFACE 
		$<$<CXX_COMPILER_ID:Clang>: /usr/include/libcxxabi/>
	)

list(APPEND PASS_TO_BOOTSTRAP 
		LIBS_DIR
		GTEST_ROOT
		BOOST_ROOT
	)

if (WIN32)
	set(LIBS_DIR C:/Libs CACHE PATH "Custom library directory")

	set(VKGLFORMAT_INCLUDE ${LIBS_DIR}/vkglformat/include CACHE PATH "Include path for vkglformat.")

elseif(UNIX)
	set(LIBS_DIR /usr/local CACHE PATH "Custom library directory")
	set(VKGLFORMAT_INCLUDE ${LIBS_DIR}/include CACHE PATH "Include path for vkglformat.")
endif()

if(SANITIZER_INSTRUMENTATION AND UNIX)
	message(STATUS "Using sanitizer instrumentation.")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
	if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize=null")
	elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize-blacklist=${CMAKE_CURRENT_SOURCE_DIR}/sanitizer_blacklist.txt")
	endif ()
endif()

add_library(deps.vkglformat INTERFACE)
target_compile_definitions(deps.vkglformat INTERFACE
	)
target_include_directories(deps.vkglformat SYSTEM INTERFACE
		${VKGLFORMAT_INCLUDE}
	)
target_compile_options(deps.vkglformat INTERFACE
	)

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

list(APPEND CMAKE_PREFIX_PATH ${LIBS_DIR} ${LIBS_DIR}/${compiler_name})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake_helpers")

find_package(gli REQUIRED)
find_package(glfw3 REQUIRED)

include(${CMAKE_CURRENT_LIST_DIR}/SetupZLIB.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/SetupGLM.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/SetupVulkan.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/SetupGTest.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/SetupBoost.cmake)

find_package(TBB REQUIRED)
