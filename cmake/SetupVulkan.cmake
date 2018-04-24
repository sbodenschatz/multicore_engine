function(find_vulkan_wrap)
	if(WIN32)
		set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX} ${CMAKE_EXTRA_LINK_EXTENSIONS})
	endif()
	find_package(Vulkan REQUIRED)
endfunction()
find_vulkan_wrap()

set_property(
	TARGET Vulkan::Vulkan
	APPEND
	PROPERTY INTERFACE_COMPILE_DEFINITIONS
		GLFW_INCLUDE_VULKAN
	)

