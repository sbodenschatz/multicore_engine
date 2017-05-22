/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/window.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_WINDOW_HPP_
#define GRAPHICS_WINDOW_HPP_

/**
 * \file
 * Defines the window handling for the graphics subsystem.
 */

#ifdef MULTICORE_ENGINE_WINDOWS
#include <windows.h> //Fix macro redefinition error in glfw header
#endif				 // MULTICORE_ENGINE_WINDOWS
#include "unique_handle.hpp"
#include <mce/glfw/window.hpp>
#include <memory>

namespace mce {
namespace graphics {
class application_instance;

/// Implements the window handling for the graphics subsystem (mainly surface creation).
class window {
private:
	application_instance& app_instance;
	glfw::window& window_;
	unique_handle<vk::SurfaceKHR> surface_;

public:
	/// Initializes a graphics window from the given graphics application_instance in the given glfw::window.
	window(application_instance& app_instance, glfw::window& win);
	/// Releases the graphics window resources.
	~window();

	/// Returns the vulkan surface held by this window.
	const vk::SurfaceKHR& surface() const {
		return *surface_;
	}

	/// Returns the glfw::window in which this graphics::window operates.
	glfw::window& glfw_window() {
		return window_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_WINDOW_HPP_ */
