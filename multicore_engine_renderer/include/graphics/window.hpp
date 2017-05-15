/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/graphics/window.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef GRAPHICS_WINDOW_HPP_
#define GRAPHICS_WINDOW_HPP_

#ifdef MULTICORE_ENGINE_WINDOWS
#include <windows.h> //Fix macro redefinition error in glfw header
#endif				 // MULTICORE_ENGINE_WINDOWS
#include "unique_handle.hpp"
#include <GLFW/glfw3.h>
#include <memory>

namespace mce {
namespace graphics {
class application_instance;

class window {
private:
	application_instance& app_instance;
	vk::Extent2D resolution_;
	std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> native_window_;
	unique_handle<vk::SurfaceKHR> surface_;

public:
	explicit window(application_instance& app_instance);
	~window();

	const vk::SurfaceKHR& surface() const {
		return *surface_;
	}
	const vk::Extent2D& resolution() const {
		return resolution_;
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* GRAPHICS_WINDOW_HPP_ */
