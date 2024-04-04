include_guard()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

include(BuildTypeSettings)

include(LibsDir)

if (MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX /ignore:4221 /ignore:4099 notelemetry.obj")
	set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /WX /ignore:4221 notelemetry.obj")

	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /debug:fastlink /INCREMENTAL")
endif ()

include(CustomLTO)

include(SanitizerInstrumentationOption)

include(MCECompilerSettings)

option(MCE_VKGLFORMAT_AS_SUBDIRECTORY "Use vkglformat as an embedded subdirectory (uses find_package otherwise)." ON)
if(MCE_VKGLFORMAT_AS_SUBDIRECTORY)
	add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../vkglformat vkglformat)
else()
	find_package(vkglformat)
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(gli REQUIRED)
find_package(glfw3 REQUIRED)

include(SetupZLIB)
include(SetupVulkan)
include(SetupGLM)
include(SetupGTest)
include(SetupBoost)

find_package(TBB REQUIRED)
