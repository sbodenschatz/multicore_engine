/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw_wrapper/cursor.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <GLFW/glfw3.h>
#include <mce/glfw/cursor.hpp>

namespace mce {
namespace glfw {

cursor::cursor(instance&, glm::ivec2 size, unsigned char* pixels, glm::ivec2 hot_spot)
		: cursor_{nullptr, [](GLFWcursor*) {}} {
	GLFWimage img{size.x, size.y, pixels};
#ifndef DOXYGEN /// Workaround for doxygen wrongly warning about an undocumented member here
	// cppcheck-suppress useInitializationList
	cursor_ = std::unique_ptr<GLFWcursor, void (*)(GLFWcursor*)>{
			glfwCreateCursor(&img, hot_spot.x, hot_spot.y), [](GLFWcursor* c) { glfwDestroyCursor(c); }};
#endif
}
cursor::cursor(instance&, standard_cursor standard_cursor_type)
		: cursor_{glfwCreateStandardCursor(static_cast<int>(standard_cursor_type)),
				  [](GLFWcursor* c) { glfwDestroyCursor(c); }} {}
cursor::cursor() : cursor_{nullptr, [](GLFWcursor*) {}} {}
cursor::~cursor() {}

} /* namespace glfw_wrapper */
} /* namespace mce */
