/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw_wrapper/window.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <GLFW/glfw3.h>
#include <exceptions.hpp>
#include <glfw_wrapper/instance.hpp>
#include <glfw_wrapper/window.hpp>

namespace mce {
namespace glfw_wrapper {

window::window(const std::string& title, const glm::ivec2& size)
		: instance_{std::make_unique<instance>()},
		  window_{std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>(nullptr, [](GLFWwindow*) {})},
		  callbacks_{std::make_unique<window_callbacks>()} {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: Make configurable
	window_ = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>(
			glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr),
			[](GLFWwindow* win) { glfwDestroyWindow(win); });
	if(!window_) throw window_creation_exception("Failed to create window.");
	setup_callbacks();
}

window::~window() {}

void window::setup_callbacks() {
	glfwSetWindowUserPointer(window_.get(), callbacks_.get());
	glfwSetKeyCallback(window_.get(), key_callback_s);
	glfwSetCharCallback(window_.get(), character_callback_s);
	glfwSetCharModsCallback(window_.get(), charmods_callback_s);
	glfwSetCursorPosCallback(window_.get(), cursor_position_callback_s);
	glfwSetCursorEnterCallback(window_.get(), cursor_enter_callback_s);
	glfwSetMouseButtonCallback(window_.get(), mouse_button_callback_s);
	glfwSetScrollCallback(window_.get(), scroll_callback_s);

	glfwSetWindowCloseCallback(window_.get(), window_close_callback_s);
	glfwSetWindowSizeCallback(window_.get(), window_size_callback_s);
	glfwSetFramebufferSizeCallback(window_.get(), framebuffer_size_callback_s);
	glfwSetWindowPosCallback(window_.get(), window_pos_callback_s);
	glfwSetWindowIconifyCallback(window_.get(), window_iconify_callback_s);
	glfwSetWindowFocusCallback(window_.get(), window_focus_callback_s);
	glfwSetWindowRefreshCallback(window_.get(), window_refresh_callback_s);
}

bool window::should_close() const {
	return glfwWindowShouldClose(window_.get());
}
void window::poll_events() {
	glfwPollEvents();
}
glm::dvec2 window::cursor_position() const {
	glm::dvec2 pos;
	glfwGetCursorPos(window_.get(), &pos.x, &pos.y);
	return pos;
}
void window::cursor_position(glm::dvec2 pos) {
	glfwSetCursorPos(window_.get(), pos.x, pos.y);
}
bool window::key(int key_code) const {
	return glfwGetKey(window_.get(), key_code) == GLFW_PRESS;
}

void window::key_callback_s(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->key;
	if(cb) cb(key, scancode, action, mods);
}
void window::character_callback_s(GLFWwindow* window, unsigned int codepoint) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->character;
	if(cb) cb(codepoint);
}
void window::charmods_callback_s(GLFWwindow* window, unsigned int codepoint, int mods) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->charmods;
	if(cb) cb(codepoint, mods);
}
void window::cursor_position_callback_s(GLFWwindow* window, double xpos, double ypos) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->cursor_position;
	if(cb) cb(xpos, ypos);
}
void window::cursor_enter_callback_s(GLFWwindow* window, int entered) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->cursor_enter;
	if(cb) cb(entered);
}
void window::mouse_button_callback_s(GLFWwindow* window, int button, int action, int mods) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->mouse_button;
	if(cb) cb(button, action, mods);
}
void window::scroll_callback_s(GLFWwindow* window, double xoffset, double yoffset) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->scroll;
	if(cb) cb(xoffset, yoffset);
}

void window::window_close_callback_s(GLFWwindow* window) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->window_close;
	if(cb) cb();
}
void window::window_size_callback_s(GLFWwindow* window, int width, int height) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->window_size;
	if(cb) cb(width, height);
}
void window::framebuffer_size_callback_s(GLFWwindow* window, int width, int height) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->framebuffer_size;
	if(cb) cb(width, height);
}
void window::window_pos_callback_s(GLFWwindow* window, int xpos, int ypos) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->window_pos;
	if(cb) cb(xpos, ypos);
}
void window::window_iconify_callback_s(GLFWwindow* window, int iconified) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->window_iconify;
	if(cb) cb(iconified == GLFW_TRUE);
}
void window::window_focus_callback_s(GLFWwindow* window, int focused) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->window_focus;
	if(cb) cb(focused == GLFW_TRUE);
}
void window::window_refresh_callback_s(GLFWwindow* window) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->window_refresh;
	if(cb) cb();
}

} /* namespace glfw_wrapper */
} /* namespace mce */
