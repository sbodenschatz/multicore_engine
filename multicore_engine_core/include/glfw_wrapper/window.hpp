/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/window.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_WINDOW_HPP_
#define GLFW_WRAPPER_WINDOW_HPP_

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace mce {
namespace glfw_wrapper {
class instance;

class window {
	struct window_callbacks {
		window_callbacks() = default;
		window_callbacks(const window_callbacks&) = delete;
		window_callbacks& operator=(const window_callbacks&) = delete;
	};
	std::unique_ptr<instance> instance_;
	std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> window_;
	std::unique_ptr<window_callbacks> callbacks_;

public:
	window(const std::string& title, const glm::ivec2& size);
	~window();
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_WINDOW_HPP_ */
