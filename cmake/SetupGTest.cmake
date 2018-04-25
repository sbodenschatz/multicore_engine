include_guard()

if(NOT DEFINED GTEST_ROOT)
	set(GTEST_ROOT ${LIBS_DIR}/gtest)
endif()
find_package(GTest REQUIRED)

set_property(
	TARGET GTest::GTest
	APPEND
	PROPERTY INTERFACE_COMPILE_DEFINITIONS
		GTEST_LANG_CXX11
	)
