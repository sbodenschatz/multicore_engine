include_guard()

if(NOT DEFINED BOOST_ROOT)
	set(BOOST_ROOT ${LIBS_DIR}/Boost)
endif()
find_package(Boost REQUIRED
		COMPONENTS
			thread
			filesystem
			program_options
	)

set_property(
	TARGET Boost::boost
	APPEND
	PROPERTY INTERFACE_COMPILE_DEFINITIONS
		$<$<CXX_COMPILER_ID:MSVC>:_HAS_AUTO_PTR_ETC>
		#The following 2 defines are needed to extend the allowed parameter count for boost::variant to the required amount.
		BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
		BOOST_MPL_LIMIT_LIST_SIZE=30
		$<$<CXX_COMPILER_ID:Clang>:_LIBCPP_ENABLE_CXX17_REMOVED_FEATURES>
		BOOST_NO_AUTO_PTR
	)

add_library(Boost.asio INTERFACE)
target_link_libraries(Boost.asio INTERFACE Boost::boost Boost::system $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<PLATFORM_ID:Windows>>:ws2_32 mswsock>)
add_library(Boost::asio ALIAS Boost.asio)

