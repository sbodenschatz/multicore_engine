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
class application_instance;
class device;

/// Implements the window handling for the graphics subsystem.
class window {
private:
	application_instance& app_instance;
	glfw::window& window_;
	device& device_;
	vk::UniqueSurfaceKHR surface_;
	vk::UniqueSwapchainKHR swapchain_;
	vk::ColorSpaceKHR color_space_;
	vk::Format surface_format_;
	vk::PresentModeKHR present_mode_;
	glm::uvec2 swapchain_size_;
	std::vector<vk::Image> swapchain_images_;

	void create_surface();
	void select_present_mode();
	void configure_surface_format();
	void create_swapchain();

public:
	/// \brief Initializes a graphics window from the given graphics application_instance in the given
	/// glfw::window using the given device.
	window(application_instance& app_instance, glfw::window& win, device& dev);
	/// Releases the graphics window resources.
	~window();

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
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_WINDOW_HPP_ */
