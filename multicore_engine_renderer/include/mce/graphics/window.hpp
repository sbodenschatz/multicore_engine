/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/window.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_WINDOW_HPP_
#define GRAPHICS_WINDOW_HPP_

/**
 * \file
 * Defines the window handling for the graphics subsystem.
 */

#include <mce/glfw/window.hpp>
#include <memory>

namespace mce {
namespace graphics {
class instance;
class device;
class framebuffer_attachment_config;
class framebuffer_pass_config;
class framebuffer_config;

/// Implements the window handling for the graphics subsystem.
class window {
private:
	std::vector<vk::PresentModeKHR> present_mode_preferences_;
	instance& instance_;
	glfw::window& window_;
	device& device_;
	uint32_t desired_images_;
	vk::UniqueSurfaceKHR surface_;
	vk::UniqueSwapchainKHR swapchain_;
	vk::ColorSpaceKHR color_space_;
	vk::Format surface_format_;
	vk::PresentModeKHR present_mode_;
	glm::uvec2 swapchain_size_;
	std::vector<vk::Image> swapchain_images_;
	std::vector<vk::UniqueImageView> swapchain_image_views_;

	void create_surface();
	void select_present_mode();
	void configure_surface_format();
	void create_swapchain();

public:
	/// \brief Initializes a graphics window from the given graphics instance in the given glfw::window using
	/// the given device.
	window(instance& app_instance, glfw::window& win, device& dev, uint32_t desired_images = 3u,
		   std::vector<vk::PresentModeKHR> present_mode_preferences = {
				   vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eFifoRelaxed, vk::PresentModeKHR::eFifo,
				   vk::PresentModeKHR::eImmediate});
	/// Releases the graphics window resources.
	~window();

	/// \brief Creates a framebuffer_config containing a swapchain image from the swapchain belonging to this
	/// window as the first element.
	framebuffer_config
	make_framebuffer_config(std::vector<framebuffer_attachment_config> additional_attachments,
							std::vector<framebuffer_pass_config> passes);

	/// Returns the vulkan surface held by this window.
	const vk::SurfaceKHR& surface() const {
		return *surface_;
	}

	/// Returns the glfw::window in which this graphics::window operates.
	const glfw::window& glfw_window() const {
		return window_;
	}

	/// Returns the used surface format.
	vk::Format surface_format() const {
		return surface_format_;
	}

	/// Returns a handle to the swapchain created for the window.
	const vk::SwapchainKHR& swapchain() const {
		return *swapchain_;
	}

	/// Returns the used color space.
	vk::ColorSpaceKHR color_space() const {
		return color_space_;
	}

	/// Returns the used present mode.
	vk::PresentModeKHR present_mode() const {
		return present_mode_;
	}

	/// Returns the size of the swapchain.
	const glm::uvec2& swapchain_size() const {
		return swapchain_size_;
	}

	/// Allows read access to the collection of swapchain images.
	const std::vector<vk::Image>& swapchain_images() const {
		return swapchain_images_;
	}

	/// Allows read access to the collection of views on the swapchain images.
	const std::vector<vk::UniqueImageView>& swapchain_image_views() const {
		return swapchain_image_views_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_WINDOW_HPP_ */
