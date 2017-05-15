/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw_wrapper/window.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <exceptions.hpp>
#include <glfw_wrapper/instance.hpp>
#include <glfw_wrapper/window.hpp>

namespace mce {
namespace glfw_wrapper {

window::window(const std::string& title, const glm::vec2& size)
		: window_{std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>(nullptr, [](GLFWwindow*) {})},
		  instance_{std::make_unique<instance>()} {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: Make configurable
	window_ = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>(
			glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr),
			[](GLFWwindow* win) { glfwDestroyWindow(win); });
	if(!window_) throw window_creation_exception("Failed to create window.");
}

window::~window() {}

} /* namespace glfw_wrapper */
} /* namespace mce */
