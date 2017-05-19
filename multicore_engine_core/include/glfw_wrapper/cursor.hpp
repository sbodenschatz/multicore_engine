/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/cursor.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_CURSOR_HPP_
#define GLFW_WRAPPER_CURSOR_HPP_

#include <memory>
#include <glm/glm.hpp>
#include <glfw_wrapper/glfw_defs.hpp>

struct GLFWcursor;

namespace mce {
namespace glfw_wrapper {
class instance;

class cursor {
	std::unique_ptr<GLFWcursor, void (*)(GLFWcursor*)> cursor_;

	friend class window;

public:
	cursor(instance&, glm::ivec2 size, unsigned char* pixels, glm::ivec2 hot_spot = {0, 0});
	cursor(instance&, standard_cursor standard_cursor_type);
	cursor();
	~cursor();
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_CURSOR_HPP_ */
