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

window::window(const std::string& title, const glm::ivec2& size, window_hint_flags hints)
		: instance_{std::make_unique<instance>()},
		  window_{std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)>(nullptr, [](GLFWwindow*) {})},
		  callbacks_{std::make_unique<window_callbacks>()} {
	set_window_hints(hints);
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

void window::set_window_hints(window_hint_flags hints) {
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, (hints & window_hint_bits::resizable) ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, (hints & window_hint_bits::visible) ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, (hints & window_hint_bits::decorated) ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_FOCUSED, (hints & window_hint_bits::focused) ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_AUTO_ICONIFY, (hints & window_hint_bits::auto_iconify) ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_FLOATING, (hints & window_hint_bits::floating) ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, (hints & window_hint_bits::maximized) ? GLFW_TRUE : GLFW_FALSE);
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
bool window::key(glfw_wrapper::key key_code) const {
	return glfwGetKey(window_.get(), static_cast<int>(key_code)) == GLFW_PRESS;
}
bool window::mouse_button(glfw_wrapper::mouse_button button) const {
	return glfwGetMouseButton(window_.get(), static_cast<int>(button)) == GLFW_PRESS;
}

void window::key_callback_s(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->key;
	if(cb)
		cb(static_cast<glfw_wrapper::key>(key), scancode, static_cast<glfw_wrapper::action>(action),
		   modifier_flags(mods));
}
void window::character_callback_s(GLFWwindow* window, unsigned int codepoint) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->character;
	if(cb) cb(codepoint);
}
void window::charmods_callback_s(GLFWwindow* window, unsigned int codepoint, int mods) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->charmods;
	if(cb) cb(codepoint, modifier_flags(mods));
}
void window::cursor_position_callback_s(GLFWwindow* window, double xpos, double ypos) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->cursor_position;
	if(cb) cb(xpos, ypos);
}
void window::cursor_enter_callback_s(GLFWwindow* window, int entered) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->cursor_enter;
	if(cb) cb(entered == GLFW_TRUE);
}
void window::mouse_button_callback_s(GLFWwindow* window, int button, int action, int mods) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->mouse_button;
	if(cb)
		cb(static_cast<glfw_wrapper::mouse_button>(button), static_cast<glfw_wrapper::action>(action),
		   modifier_flags(mods));
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
