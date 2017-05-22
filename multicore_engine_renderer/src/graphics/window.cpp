/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/window.cpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifdef MULTICORE_ENGINE_WINDOWS
#include <windows.h> //Fix macro redefinition error in glfw header
#endif

#include <GLFW/glfw3.h>
#include <cstdint>
#include <iostream>
#include <mce/exceptions.hpp>
#include <mce/graphics/application_instance.hpp>
#include <mce/graphics/device.hpp>
#include <mce/graphics/window.hpp>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

window::window(application_instance& app_instance, glfw::window& win, device& dev)
		: app_instance(app_instance), window_{win}, device_{dev}, surface_format_{vk::Format::eUndefined} {
	create_surface();
	create_swapchain();
}

window::~window() {}

void window::create_surface() {
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
	if(!device_.physical_device().getSurfaceSupportKHR(device_.present_queue_index().first, surface_.get())) {
		throw window_surface_creation_exception("Surface not supported by device.");
	}
}

void window::create_swapchain() {
	vk::PresentModeKHR present_mode = vk::PresentModeKHR::eFifo; // Fifo is required to be available by spec.
	// Check if fifo relaxed mode is available
	std::vector<vk::PresentModeKHR> present_modes =
			device_.physical_device().getSurfacePresentModesKHR(surface_.get());
	if(std::find(present_modes.begin(), present_modes.end(), vk::PresentModeKHR::eFifoRelaxed) !=
	   present_modes.end()) {
		present_mode = vk::PresentModeKHR::eFifoRelaxed;
	}
	for(const auto& pm : present_modes) std::cout << vk::to_string(pm) << std::endl;

	std::vector<vk::SurfaceFormatKHR> surface_formats =
			device_.physical_device().getSurfaceFormatsKHR(surface_.get());
	vk::ColorSpaceKHR color_space = vk::ColorSpaceKHR::eSrgbNonlinear;
	surface_format_ = vk::Format::eB8G8R8A8Unorm;
	if(surface_formats.size() != 1 || surface_formats[0].format != vk::Format::eUndefined) {
		std::vector<vk::Format> format_preferences = {// TODO Place other preferred formats here
													  vk::Format::eB8G8R8A8Unorm, vk::Format::eB8G8R8A8Srgb};
		std::stable_sort(surface_formats.begin(), surface_formats.end(),
						 [&format_preferences](const auto& v0, const auto& v1) {
							 auto pref = [&format_preferences](auto x) {
								 return std::find(format_preferences.begin(), format_preferences.end(), x);
							 };
							 return pref(v0.format) < pref(v1.format);
						 });
		color_space = surface_formats[0].colorSpace;
		surface_format_ = surface_formats[0].format;
	}
	for(const auto& sf : surface_formats)
		std::cout << vk::to_string(sf.colorSpace) << " " << vk::to_string(sf.format) << std::endl;

	vk::SurfaceCapabilitiesKHR surface_caps =
			device_.physical_device().getSurfaceCapabilitiesKHR(surface_.get());

	vk::SwapchainCreateInfoKHR swapchain_ci;
	swapchain_ci.surface = surface_.get();
	uint32_t image_count = surface_caps.minImageCount + 2;

	if(surface_caps.maxImageCount > 0 && image_count > surface_caps.maxImageCount)
		image_count = surface_caps.maxImageCount;
	swapchain_ci.minImageCount = image_count;
	swapchain_ci.imageFormat = surface_format_;
	swapchain_ci.imageColorSpace = color_space;

	auto resolution = window_.framebuffer_size();
	vk::Extent2D swapchain_size = vk::Extent2D{uint32_t(resolution.x), uint32_t(resolution.y)};
	if(surface_caps.currentExtent.width != ~0u) {
		swapchain_size = surface_caps.currentExtent;
	}
	swapchain_ci.imageExtent = swapchain_size;
	swapchain_ci.imageArrayLayers = 1;
	swapchain_ci.imageSharingMode = vk::SharingMode::eExclusive;
	swapchain_ci.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapchain_ci.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	swapchain_ci.preTransform = surface_caps.currentTransform;
	swapchain_ci.clipped = true;
	swapchain_ci.presentMode = present_mode;

	swapchain_ = unique_handle<vk::SwapchainKHR, true>(
			device_.native_device().createSwapchainKHR(swapchain_ci),
			[this](vk::SwapchainKHR& swapchain, const vk::Optional<const vk::AllocationCallbacks>& alloc) {
				device_.native_device().destroySwapchainKHR(swapchain, alloc);
			});
}

} /* namespace graphics */
} /* namespace mce */
