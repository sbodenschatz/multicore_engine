if(NOT MSVC)
	if(NOT (CMAKE_BUILD_TYPE STREQUAL Debug) AND NOT (CMAKE_BUILD_TYPE STREQUAL Release))
		set(CMAKE_BUILD_TYPE Release)
	endif()
	string(TOLOWER ${CMAKE_BUILD_TYPE} CONFIG_LOWER)
endif()

detect_compiler_name()

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

include(${CMAKE_CURRENT_LIST_DIR}/MCECompilerSettings.cmake)

list(APPEND PASS_TO_BOOTSTRAP 
		LIBS_DIR
		GTEST_ROOT
		BOOST_ROOT
	)

if (WIN32)
	set(LIBS_DIR C:/Libs CACHE PATH "Custom library directory")
elseif(UNIX)
	set(LIBS_DIR /usr/local CACHE PATH "Custom library directory")
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

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../vkglformat vkglformat)


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
