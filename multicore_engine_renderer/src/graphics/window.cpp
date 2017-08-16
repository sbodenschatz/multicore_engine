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
#include <mce/graphics/device.hpp>
#include <mce/graphics/instance.hpp>
#include <mce/graphics/window.hpp>
#include <mce/util/algorithm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace mce {
namespace graphics {

window::window(instance& app_instance, glfw::window& win, device& dev)
		: instance_{app_instance}, window_{win}, device_{dev},
		  color_space_{vk::ColorSpaceKHR::eSrgbNonlinear}, surface_format_{vk::Format::eUndefined},
		  present_mode_{vk::PresentModeKHR::eFifo} {
	create_surface();
	select_present_mode();
	configure_surface_format();
	create_swapchain();
}

window::~window() {}

void window::create_surface() {
	VkSurfaceKHR surface_tmp;
	if(glfwCreateWindowSurface(instance_.native_instance(), window_.window_.get(), nullptr, &surface_tmp) !=
	   VK_SUCCESS) {
		throw window_surface_creation_exception("Failed to create window surface.");
	}
	surface_ = vk::UniqueSurfaceKHR(surface_tmp, instance_.native_instance());
	if(!device_.physical_device().getSurfaceSupportKHR(device_.present_queue_index().first, surface_.get())) {
		throw window_surface_creation_exception("Surface not supported by device.");
	}
}

void window::configure_surface_format() {
	std::vector<vk::SurfaceFormatKHR> surface_formats =
			device_.physical_device().getSurfaceFormatsKHR(surface_.get());
	color_space_ = vk::ColorSpaceKHR::eSrgbNonlinear;
	surface_format_ = vk::Format::eB8G8R8A8Unorm;
	if(surface_formats.size() != 1 || surface_formats[0].format != vk::Format::eUndefined) {
		std::vector<vk::Format> format_preferences = {// TODO Place other preferred formats here
													  vk::Format::eB8G8R8A8Unorm};
		util::preference_sort(surface_formats, format_preferences, [](const auto& v) { return v.format; });
		color_space_ = surface_formats[0].colorSpace;
		surface_format_ = surface_formats[0].format;
	}
	for(const auto& sf : surface_formats)
		std::cout << vk::to_string(sf.colorSpace) << " " << vk::to_string(sf.format) << std::endl;
}

void window::select_present_mode() {
	present_mode_ = vk::PresentModeKHR::eFifo; // Fifo is required to be available by spec.
	// Check if fifo relaxed mode is available
	std::vector<vk::PresentModeKHR> present_modes =
			device_.physical_device().getSurfacePresentModesKHR(surface_.get());
	if(std::find(present_modes.begin(), present_modes.end(), vk::PresentModeKHR::eFifoRelaxed) !=
	   present_modes.end()) {
		present_mode_ = vk::PresentModeKHR::eFifoRelaxed;
	}
	if(std::find(present_modes.begin(), present_modes.end(), vk::PresentModeKHR::eMailbox) !=
	   present_modes.end()) {
		present_mode_ = vk::PresentModeKHR::eMailbox;
	}
	for(const auto& pm : present_modes) std::cout << vk::to_string(pm) << std::endl;
}

void window::create_swapchain() {

	vk::SurfaceCapabilitiesKHR surface_caps =
			device_.physical_device().getSurfaceCapabilitiesKHR(surface_.get());

	vk::SwapchainCreateInfoKHR swapchain_ci;
	swapchain_ci.surface = surface_.get();
	uint32_t image_count = std::max(surface_caps.minImageCount, 3u);

	if(surface_caps.maxImageCount > 0 && image_count > surface_caps.maxImageCount)
		image_count = surface_caps.maxImageCount;
	swapchain_ci.minImageCount = image_count;
	swapchain_ci.imageFormat = surface_format_;
	swapchain_ci.imageColorSpace = color_space_;

	auto resolution = window_.framebuffer_size();
	vk::Extent2D swapchain_size_ext = vk::Extent2D{uint32_t(resolution.x), uint32_t(resolution.y)};
	swapchain_size_ext.width =
			std::min(surface_caps.maxImageExtent.width,
					 std::max(surface_caps.minImageExtent.width, swapchain_size_ext.width));
	swapchain_size_ext.height =
			std::min(surface_caps.maxImageExtent.height,
					 std::max(surface_caps.minImageExtent.height, swapchain_size_ext.height));
	if(surface_caps.currentExtent.width != ~0u) {
		swapchain_size_ext = surface_caps.currentExtent;
	}
	swapchain_ci.imageExtent = swapchain_size_ext;
	swapchain_ci.imageArrayLayers = 1;
	swapchain_ci.imageSharingMode = vk::SharingMode::eExclusive;
	swapchain_ci.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapchain_ci.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	swapchain_ci.preTransform = surface_caps.currentTransform;
	swapchain_ci.clipped = true;
	swapchain_ci.presentMode = present_mode_;

	swapchain_ = device_->createSwapchainKHRUnique(swapchain_ci);
	swapchain_size_ = {swapchain_size_ext.width, swapchain_size_ext.height};
	swapchain_images_ = device_->getSwapchainImagesKHR(swapchain_.get());
	for(auto& sci : swapchain_images_) {
		swapchain_image_views_.push_back(device_->createImageViewUnique(vk::ImageViewCreateInfo(
				{}, sci, vk::ImageViewType::e2D, surface_format_, {},
				vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))));
	}
}

framebuffer_config
window::make_framebuffer_config(std::vector<framebuffer_attachment_config> additional_attachments) {
	framebuffer_config res{std::move(additional_attachments)};
	framebuffer_attachment_config swapchain_attachment(surface_format_);
	swapchain_attachment.is_swapchain_image_ = true;
	res.attachment_configs_.insert(res.attachment_configs_.begin(), swapchain_attachment);
	return res;
}

} /* namespace graphics */
} /* namespace mce */
