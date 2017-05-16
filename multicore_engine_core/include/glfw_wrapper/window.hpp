/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/window.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_WINDOW_HPP_
#define GLFW_WRAPPER_WINDOW_HPP_

#include <functional>
#include <glfw_wrapper/glfw_defs.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

namespace mce {
namespace glfw_wrapper {
class instance;

class window {
	void setup_callbacks();
	static void key_callback_s(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void character_callback_s(GLFWwindow* window, unsigned int codepoint);
	static void charmods_callback_s(GLFWwindow* window, unsigned int codepoint, int mods);
	static void cursor_position_callback_s(GLFWwindow* window, double xpos, double ypos);
	static void cursor_enter_callback_s(GLFWwindow* window, int entered);
	static void mouse_button_callback_s(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback_s(GLFWwindow* window, double xoffset, double yoffset);

	static void window_close_callback_s(GLFWwindow* window);
	static void window_size_callback_s(GLFWwindow* window, int width, int height);
	static void framebuffer_size_callback_s(GLFWwindow* window, int width, int height);
	static void window_pos_callback_s(GLFWwindow* window, int xpos, int ypos);
	static void window_iconify_callback_s(GLFWwindow* window, int iconified);
	static void window_focus_callback_s(GLFWwindow* window, int focused);
	static void window_refresh_callback_s(GLFWwindow* window);
	static void window_drop_callback_s(GLFWwindow* window, int count, const char** paths);

	typedef std::function<void(key key, int scancode, action action, modifier_flags mods)> key_callback_t;
	typedef std::function<void(unsigned int codepoint)> character_callback_t;
	typedef std::function<void(unsigned int codepoint, modifier_flags mods)> charmods_callback_t;
	typedef std::function<void(double xpos, double ypos)> cursor_position_callback_t;
	typedef std::function<void(bool entered)> cursor_enter_callback_t;
	typedef std::function<void(mouse_button button, action action, modifier_flags mods)>
			mouse_button_callback_t;
	typedef std::function<void(double xoffset, double yoffset)> scroll_callback_t;

	typedef std::function<void()> window_close_callback_t;
	typedef std::function<void(int width, int height)> window_size_callback_t;
	typedef std::function<void(int width, int height)> framebuffer_size_callback_t;
	typedef std::function<void(int xpos, int ypos)> window_pos_callback_t;
	typedef std::function<void(bool iconified)> window_iconify_callback_t;
	typedef std::function<void(bool focused)> window_focus_callback_t;
	typedef std::function<void()> window_refresh_callback_t;
	typedef std::function<void(const std::vector<std::string>& paths)> window_drop_callback_t;

	struct window_callbacks {
		window_callbacks() = default;
		window_callbacks(const window_callbacks&) = delete;
		window_callbacks& operator=(const window_callbacks&) = delete;
		key_callback_t key;
		character_callback_t character;
		charmods_callback_t charmods;
		cursor_position_callback_t cursor_position;
		cursor_enter_callback_t cursor_enter;
		mouse_button_callback_t mouse_button;
		scroll_callback_t scroll;

		window_close_callback_t window_close;
		window_size_callback_t window_size;
		framebuffer_size_callback_t framebuffer_size;
		window_pos_callback_t window_pos;
		window_iconify_callback_t window_iconify;
		window_focus_callback_t window_focus;
		window_refresh_callback_t window_refresh;
		window_drop_callback_t window_drop;
	};

	std::unique_ptr<instance> instance_;
	std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> window_;
	std::unique_ptr<window_callbacks> callbacks_;

	void set_window_hints(window_hint_flags hints);

public:
	window(const std::string& title, const glm::ivec2& size,
		   window_hint_flags hints = window_hint_flags() | window_hint_bits::visible |
									 window_hint_bits::decorated | window_hint_bits::focused |
									 window_hint_bits::auto_iconify);
	~window();

	bool should_close() const;
	static void poll_events();
	glm::dvec2 cursor_position() const;
	void cursor_position(glm::dvec2 pos);
	bool key(key key_code) const;
	bool mouse_button(mouse_button button) const;

	glm::ivec2 window_position() const;
	void window_position(glm::ivec2 pos);
	glm::ivec2 window_size() const;
	void window_size(glm::ivec2 size);
	glm::ivec2 framebuffer_size() const;
	struct window_frame {
		int left;
		int top;
		int right;
		int bottom;
	};
	window_frame window_frame_size() const;
	void window_size_limits(glm::ivec2 min = {dont_care, dont_care}, glm::ivec2 max = {dont_care, dont_care});
	void aspect_ratio(int numerator = dont_care, int denominator = dont_care);

	glfw_wrapper::cursor_mode cursor_mode() const;
	void cursor_mode(glfw_wrapper::cursor_mode mode);
	bool sticky_keys() const;
	void sticky_keys(bool enabled);
	bool sticky_mouse_buttons() const;
	void sticky_mouse_buttons(bool enabled);
	std::string clipboard() const;
	void clipboard(const std::string& content);

	void iconify();
	void restore();
	void maximize();
	void hide();
	void focus();

	template <typename F>
	void key_callback(F&& f) {
		callbacks_->key = std::forward<F>(f);
	}
	template <typename F>
	void character_callback(F&& f) {
		callbacks_->character = std::forward<F>(f);
	}
	template <typename F>
	void character_mods_callback(F&& f) {
		callbacks_->charmods = std::forward<F>(f);
	}
	template <typename F>
	void cursor_position_callback(F&& f) {
		callbacks_->cursor_position = std::forward<F>(f);
	}
	template <typename F>
	void cursor_enter_callback(F&& f) {
		callbacks_->cursor_enter = std::forward<F>(f);
	}
	template <typename F>
	void mouse_button_callback(F&& f) {
		callbacks_->mouse_button = std::forward<F>(f);
	}
	template <typename F>
	void scroll_callback(F&& f) {
		callbacks_->scroll = std::forward<F>(f);
	}
	template <typename F>
	void window_close_callback(F&& f) {
		callbacks_->window_close = std::forward<F>(f);
	}
	template <typename F>
	void window_size_callback(F&& f) {
		callbacks_->window_size = std::forward<F>(f);
	}
	template <typename F>
	void framebuffer_size_callback(F&& f) {
		callbacks_->framebuffer_size = std::forward<F>(f);
	}
	template <typename F>
	void window_position_callback(F&& f) {
		callbacks_->window_pos = std::forward<F>(f);
	}
	template <typename F>
	void window_iconify_callback(F&& f) {
		callbacks_->window_iconify = std::forward<F>(f);
	}
	template <typename F>
	void window_focus_callback(F&& f) {
		callbacks_->window_focus = std::forward<F>(f);
	}
	template <typename F>
	void window_refresh_callback(F&& f) {
		callbacks_->window_refresh = std::forward<F>(f);
	}
	template <typename F>
	void window_drop_callback(F&& f) {
		callbacks_->window_drop = std::forward<F>(f);
	}
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_WINDOW_HPP_ */
