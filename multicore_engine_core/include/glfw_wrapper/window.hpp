/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/window.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_WINDOW_HPP_
#define GLFW_WRAPPER_WINDOW_HPP_

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <functional>

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

	// TODO Change signatures to typesafe parameters:
	typedef std::function<void(int key, int scancode, int action, int mods)> key_callback_t;
	typedef std::function<void(unsigned int codepoint)> character_callback_t;
	typedef std::function<void(unsigned int codepoint, int mods)> charmods_callback_t;
	typedef std::function<void(double xpos, double ypos)> cursor_position_callback_t;
	typedef std::function<void(int entered)> cursor_enter_callback_t;
	typedef std::function<void(int button, int action, int mods)> mouse_button_callback_t;
	typedef std::function<void(double xoffset, double yoffset)> scroll_callback_t;

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
	};
	std::unique_ptr<instance> instance_;
	std::unique_ptr<GLFWwindow, void (*)(GLFWwindow*)> window_;
	std::unique_ptr<window_callbacks> callbacks_;

public:
	window(const std::string& title, const glm::ivec2& size);
	~window();

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
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_WINDOW_HPP_ */
