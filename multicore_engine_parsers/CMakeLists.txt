cmake_minimum_required (VERSION 2.6)
project(multicore_engine_parsers)
file(GLOB_RECURSE PARSERS_SRC "src/*.cpp")
file(GLOB_RECURSE PARSERS_HEADERS "include/*.hpp" "include/*.h")
add_library(multicore_engine_parsers STATIC ${PARSERS_SRC} ${PARSERS_HEADERS})

if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
	set(CMAKE_CXX_FLAGS_DEBUG "")
endif ()
