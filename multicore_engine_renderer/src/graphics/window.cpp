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

namespace mce {
namespace graphics {

window::window(application_instance& app_instance)
		: app_instance(app_instance),
		  native_window_(std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>(nullptr, [](GLFWwindow*) {})) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	resolution_ = vk::Extent2D(800, 600); // TODO: Make configurable
	native_window_ = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>(
			glfwCreateWindow(resolution_.width, resolution_.height, "Test", nullptr, nullptr),
			[](GLFWwindow* win) { glfwDestroyWindow(win); });
	if(!native_window_) throw std::runtime_error("Failed to create window.");
	VkSurfaceKHR surface_tmp;
	if(glfwCreateWindowSurface(app_instance.instance(), native_window_.get(), nullptr, &surface_tmp) !=
	   VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface.");
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
