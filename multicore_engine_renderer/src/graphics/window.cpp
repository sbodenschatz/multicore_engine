/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/window.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#include <cstdint>
#include <graphics/application_instance.hpp>
#include <graphics/window.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <exceptions.hpp>

namespace mce {
namespace graphics {

window::window(application_instance& app_instance, glfw::window& win)
		: app_instance(app_instance), window_{win} {
	VkSurfaceKHR surface_tmp;
	if(glfwCreateWindowSurface(app_instance.instance(), window_.window_.get(), nullptr, &surface_tmp) !=
	   VK_SUCCESS) {
		throw window_surface_creation_exception("Failed to create window surface.");
	}
	surface_ = unique_handle<vk::SurfaceKHR>(
			vk::SurfaceKHR(surface_tmp),
			[this](vk::SurfaceKHR& surface, const vk::Optional<const vk::AllocationCallbacks>& alloc) {
				this->app_instance.instance().destroySurfaceKHR(surface, alloc);
			});
}

window::~window() {}

} /* namespace graphics */
} /* namespace mce */
