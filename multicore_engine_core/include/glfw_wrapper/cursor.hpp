/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/cursor.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_CURSOR_HPP_
#define GLFW_WRAPPER_CURSOR_HPP_

#include <glfw_wrapper/glfw_defs.hpp>
#include <glm/glm.hpp>
#include <memory>

struct GLFWcursor;

namespace mce {
namespace glfw {
class instance;

/// Represents a (mouse) cursor object in glfw.
/**
 * May only be constructed and destroyed on the main thread because of glfw limitations.
 */
class cursor {
	std::unique_ptr<GLFWcursor, void (*)(GLFWcursor*)> cursor_;

	friend class window;

public:
	/// Creates a custom cursor with the given size, pixel data and hot spot point.
	/**
	 * The pixel data is passed through to glfw, which has the following contract for the data format:
	 * "The image data is 32-bit, little-endian, non-premultiplied RGBA, i.e. eight bits per channel. The
	 * pixels are arranged canonically as sequential rows, starting from the top-left corner."
	 */
	cursor(instance&, glm::ivec2 size, unsigned char* pixels, glm::ivec2 hot_spot = {0, 0});
	/// Creates a cursor object using the given standard shape.
	cursor(instance&, standard_cursor standard_cursor_type);
	/// Creates a null-cursor.
	cursor();
	/// Destroys the cursor object.
	~cursor();
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_CURSOR_HPP_ */
