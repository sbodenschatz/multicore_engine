/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw_wrapper/cursor.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <glfw_wrapper/cursor.hpp>
#include <GLFW/glfw3.h>

namespace mce {
namespace glfw_wrapper {

cursor::cursor(instance&, glm::ivec2 size, unsigned char* pixels, glm::ivec2 hot_spot) : cursor{} {
	GLFWimage img{size.x, size.y, pixels};
	cursor_ = std::unique_ptr<GLFWcursor, void (*)(GLFWcursor*)>{
			glfwCreateCursor(&img, hot_spot.x, hot_spot.y), [](GLFWcursor* c) { glfwDestroyCursor(c); }};
}
// cppcheck-suppress noExplicitConstructor
cursor::cursor(instance&, standard_cursor standard_cursor_type)
		: cursor_{glfwCreateStandardCursor(static_cast<int>(standard_cursor_type)),
				  [](GLFWcursor* c) { glfwDestroyCursor(c); }} {}
cursor::cursor() : cursor_{nullptr, [](GLFWcursor*) {}} {}
cursor::~cursor() {}

} /* namespace glfw_wrapper */
} /* namespace mce */
