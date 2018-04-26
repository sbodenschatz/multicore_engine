include_guard()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

include(BuildTypeSettings)

include(LibsDir)

if (MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /WX /ignore:4221 notelemetry.obj")
	set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /WX /ignore:4221 notelemetry.obj")

	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /debug:fastlink /INCREMENTAL")
endif ()

include(CustomLTO)

include(SanitizerInstrumentationOption)

include(MCECompilerSettings)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../vkglformat vkglformat)

find_package(gli REQUIRED)
find_package(glfw3 REQUIRED)

include(SetupZLIB)
include(SetupGLM)
include(SetupVulkan)
include(SetupGTest)
include(SetupBoost)

find_package(TBB REQUIRED)
