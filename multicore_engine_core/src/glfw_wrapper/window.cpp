/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw_wrapper/window.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <GLFW/glfw3.h>
#include <algorithm>
#include <exceptions.hpp>
#include <glfw_wrapper/instance.hpp>
#include <glfw_wrapper/window.hpp>
#include <iterator>
#include <glfw_wrapper/monitor.hpp>
#include <glfw_wrapper/cursor.hpp>

namespace mce {
namespace glfw {

// cppcheck-suppress passedByValue
window::window(const std::string& title, const glm::ivec2& size, window_hint_flags hints)
		: instance_{std::make_unique<instance>()},
		  window_{std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>(nullptr, [](GLFWwindow*) {})},
		  callbacks_{std::make_unique<window_callbacks>()} {
	set_window_hints(hints);
	window_ = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>(
			glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr),
			[](GLFWwindow* win) { glfwDestroyWindow(win); });
	if(!window_) throw window_creation_exception("Failed to create window.");
	setup_callbacks();
}
// cppcheck-suppress passedByValue
window::window(const std::string& title, const monitor& mon, const video_mode& mode, window_hint_flags hints)
		: instance_{std::make_unique<instance>()},
		  window_{std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>(nullptr, [](GLFWwindow*) {})},
		  callbacks_{std::make_unique<window_callbacks>()} {
	set_window_hints(hints);
	glfwWindowHint(GLFW_RED_BITS, mode.red_bits);
	glfwWindowHint(GLFW_GREEN_BITS, mode.green_bits);
	glfwWindowHint(GLFW_BLUE_BITS, mode.blue_bits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode.refresh_rate);
	window_ = std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)>(
			glfwCreateWindow(mode.width, mode.height, title.c_str(), mon.monitor_, nullptr),
			[](GLFWwindow* win) { glfwDestroyWindow(win); });
	if(!window_) throw window_creation_exception("Failed to create window.");
	setup_callbacks();
}
// cppcheck-suppress passedByValue
window::window(const std::string& title, const monitor& mon, window_hint_flags hints)
		: window(title, mon, mon.current_video_mode(), hints) {}

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

// cppcheck-suppress passedByValue
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
glm::dvec2 window::cursor_position() const {
	glm::dvec2 pos;
	glfwGetCursorPos(window_.get(), &pos.x, &pos.y);
	return pos;
}
void window::cursor_position(glm::dvec2 pos) {
	glfwSetCursorPos(window_.get(), pos.x, pos.y);
}
bool window::key(glfw::key key_code) const {
	return glfwGetKey(window_.get(), static_cast<int>(key_code)) == GLFW_PRESS;
}
bool window::mouse_button(glfw::mouse_button button) const {
	return glfwGetMouseButton(window_.get(), static_cast<int>(button)) == GLFW_PRESS;
}

glfw::cursor_mode window::cursor_mode() const {
	return static_cast<glfw::cursor_mode>(glfwGetInputMode(window_.get(), GLFW_CURSOR));
}
void window::cursor_mode(glfw::cursor_mode mode) {
	glfwSetInputMode(window_.get(), GLFW_CURSOR, static_cast<int>(mode));
}
bool window::sticky_keys() const {
	return glfwGetInputMode(window_.get(), GLFW_STICKY_KEYS) == GLFW_TRUE;
}
void window::sticky_keys(bool enabled) {
	glfwSetInputMode(window_.get(), GLFW_STICKY_KEYS, enabled ? GLFW_TRUE : GLFW_FALSE);
}
bool window::sticky_mouse_buttons() const {
	return glfwGetInputMode(window_.get(), GLFW_STICKY_MOUSE_BUTTONS) == GLFW_TRUE;
}
void window::sticky_mouse_buttons(bool enabled) {
	glfwSetInputMode(window_.get(), GLFW_STICKY_MOUSE_BUTTONS, enabled ? GLFW_TRUE : GLFW_FALSE);
}
std::string window::clipboard() const {
	return glfwGetClipboardString(window_.get());
}
void window::clipboard(const std::string& content) {
	glfwSetClipboardString(window_.get(), content.c_str());
}

glm::ivec2 window::window_position() const {
	glm::ivec2 s;
	glfwGetWindowPos(window_.get(), &s.x, &s.y);
	return s;
}
void window::window_position(glm::ivec2 pos) {
	glfwSetWindowPos(window_.get(), pos.x, pos.y);
}
glm::ivec2 window::window_size() const {
	glm::ivec2 s;
	glfwGetWindowSize(window_.get(), &s.x, &s.y);
	return s;
}
void window::window_size(glm::ivec2 size) {
	glfwSetWindowSize(window_.get(), size.x, size.y);
}
glm::ivec2 window::framebuffer_size() const {
	glm::ivec2 s;
	glfwGetFramebufferSize(window_.get(), &s.x, &s.y);
	return s;
}
window::window_frame window::window_frame_size() const {
	window_frame f;
	glfwGetWindowFrameSize(window_.get(), &f.left, &f.top, &f.right, &f.bottom);
	return f;
}
void window::window_size_limits(glm::ivec2 min, glm::ivec2 max) {
	glfwSetWindowSizeLimits(window_.get(), min.x, min.y, max.x, max.y);
}
void window::aspect_ratio(int numerator, int denominator) {
	glfwSetWindowAspectRatio(window_.get(), numerator, denominator);
}
void window::aspect_ratio() {
	glfwSetWindowAspectRatio(window_.get(), dont_care, dont_care);
}

void window::iconify() {
	glfwIconifyWindow(window_.get());
}
void window::restore() {
	glfwRestoreWindow(window_.get());
}
void window::maximize() {
	glfwMaximizeWindow(window_.get());
}
void window::hide() {
	glfwHideWindow(window_.get());
}
void window::focus() {
	glfwFocusWindow(window_.get());
}
void window::cursor(const glfw::cursor& cur) {
	glfwSetCursor(window_.get(), cur.cursor_.get());
}
boost::optional<glfw::monitor> window::fullscreen_monitor() const {
	auto m = glfwGetWindowMonitor(window_.get());
	if(m) {
		return glfw::monitor(m);
	} else {
		return boost::optional<glfw::monitor>();
	}
}
void window::fullscreen_mode(glfw::monitor& mon, glm::ivec2 resolution, int refresh_rate) {
	glfwSetWindowMonitor(window_.get(), mon.monitor_, 0, 0, resolution.x, resolution.y, refresh_rate);
}
void window::windowed_mode(glm::ivec2 pos, glm::ivec2 size) {
	glfwSetWindowMonitor(window_.get(), nullptr, pos.x, pos.y, size.x, size.y, 0);
}
void window::windowed_fullscreen_mode(glfw::monitor& mon) {
	auto vm = mon.current_video_mode();
	glfwSetWindowMonitor(window_.get(), mon.monitor_, 0, 0, vm.width, vm.height, vm.refresh_rate);
}

void window::key_callback_s(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->key;
	if(cb)
		cb(static_cast<glfw::key>(key), scancode, static_cast<glfw::button_action>(action),
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
		cb(static_cast<glfw::mouse_button>(button), static_cast<glfw::button_action>(action),
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
void window::window_drop_callback_s(GLFWwindow* window, int count, const char** paths) {
	auto& cb = static_cast<window_callbacks*>(glfwGetWindowUserPointer(window))->window_drop;
	if(cb) {
		std::vector<std::string> paths_buffer;
		paths_buffer.reserve(count);
		std::copy(paths, paths + count, std::back_inserter(paths_buffer));
		cb(paths_buffer);
	}
}

} /* namespace glfw_wrapper */
} /* namespace mce */
