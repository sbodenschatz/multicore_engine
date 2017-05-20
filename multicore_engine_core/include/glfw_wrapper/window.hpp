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
#include <boost/optional.hpp>

struct GLFWwindow;

namespace mce {
namespace glfw_wrapper {
class instance;
class monitor;
class cursor;

/// Provides window related functionality of glfw by representing a single window as an object.
/**
 * Because of glfw limitation most member functions may only be called on the main thread, those that may be
 * called from other threads say so in their respective member documentation.
 */
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

	typedef std::function<void(key key, int scancode, button_action button_action, modifier_flags mods)>
			key_callback_t;
	typedef std::function<void(unsigned int codepoint)> character_callback_t;
	typedef std::function<void(unsigned int codepoint, modifier_flags mods)> charmods_callback_t;
	typedef std::function<void(double xpos, double ypos)> cursor_position_callback_t;
	typedef std::function<void(bool entered)> cursor_enter_callback_t;
	typedef std::function<void(mouse_button button, button_action button_action, modifier_flags mods)>
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
	/// Constructs a window in windowed mode with the given title, size, and behavior hints.
	window(const std::string& title, const glm::ivec2& size,
		   window_hint_flags hints = window_hint_flags() | window_hint_bits::visible |
									 window_hint_bits::decorated | window_hint_bits::focused |
									 window_hint_bits::auto_iconify);
	/// \brief Constructs a window in full screen mode with the given title, video mode and behavior hints on
	/// the given monitor.
	window(const std::string& title, const monitor& monitor, const video_mode& mode,
		   window_hint_flags hints = window_hint_flags() | window_hint_bits::auto_iconify);
	/// \brief Constructs a window in windowed full screen mode with the given title and behavior hints on the
	/// given monitor.
	window(const std::string& title, const monitor& monitor,
		   window_hint_flags hints = window_hint_flags() | window_hint_bits::auto_iconify);
	/// Destroys the window.
	~window();

	/// Returns the state of the flag, that indicates if the window schould close.
	/**
	 * May be called from any thread, but requires external synchronization.
	 */
	bool should_close() const;
	/// Returns the current position of the (mouse) cursor.
	glm::dvec2 cursor_position() const;
	/// Sets the (mouse) cursor to the given position.
	void cursor_position(glm::dvec2 pos);
	/// Returns the pressed state of the given keyboard key.
	bool key(key key_code) const;
	/// Returns the pressed state of the given mouse button.
	bool mouse_button(mouse_button button) const;

	/// Returns the position of the window in virtual screen coordinates.
	glm::ivec2 window_position() const;
	/// Sets the window to the given position in virtual screen coordinates.
	void window_position(glm::ivec2 pos);
	/// Returns the size of the window in virtual screen coordinates.
	glm::ivec2 window_size() const;
	/// Resizes the window to the given size in virtual screen coordinates.
	void window_size(glm::ivec2 size);
	/// Returns the size of the associated framebuffer in pixels.
	glm::ivec2 framebuffer_size() const;
	/// Represents measurements for the window frame.
	struct window_frame {
		int left;   ///<Size of the left frame edge.
		int top;	///<Size of the top frame edge.
		int right;  ///<Size of the right frame edge.
		int bottom; ///<Size of the bottom frame edge.
	};
	/// Returns the sizes of the window frame.
	window_frame window_frame_size() const;
	/// Sets the limits for the window size to which the the user can resize the window if it is resizable.
	/**
	 * If a direction should be unlimited, dont_care may be given.
	 */
	void window_size_limits(glm::ivec2 min = {dont_care, dont_care}, glm::ivec2 max = {dont_care, dont_care});
	/// Sets a fixed aspect ratio for the window.
	void aspect_ratio(int numerator, int denominator);
	/// Resets a fixed aspect ratio for the window.
	void aspect_ratio();

	/// Returns the current cursor mode, the window operates in.
	glfw_wrapper::cursor_mode cursor_mode() const;
	/// Sets the window to the given cursor mode.
	void cursor_mode(glfw_wrapper::cursor_mode mode);
	/// Returns a bool indicating, if sticky keys is enabled for the window.
	bool sticky_keys() const;
	/// Sets sticky keys to on or off.
	/**
	 * Sticky keys prevents polling from missing a key press by keeping a pressed and then release key in the
	 * pressed state until the next poll.
	 */
	void sticky_keys(bool enabled);
	/// Returns a bool indicating, if sticky mouse buttons is enabled for the window.
	bool sticky_mouse_buttons() const;
	/**
	 * Sticky mouse buttons prevents polling from missing a mouse button press by keeping a pressed and then
	 * release mouse button in the pressed state until the next poll.
	 */
	void sticky_mouse_buttons(bool enabled);
	/// Returns the content of the clipboard.
	std::string clipboard() const;
	/// Sets the clipboard to the given content.
	void clipboard(const std::string& content);
	/// Sets the cursor to the given cursor object.
	void cursor(const glfw_wrapper::cursor& cur);

	/// Iconifies the window.
	void iconify();
	/// Restores the window.
	void restore();
	/// Maximizes the window.
	void maximize();
	/// Hides the window.
	void hide();
	/// Sets focus to this window.
	void focus();

	/// \brief Returns the monitor the window is fullscreened on, if it is in fullscreen mode, or an empty
	/// optional if it is windowed.
	boost::optional<glfw_wrapper::monitor> fullscreen_monitor() const;
	/// Sets the window to fullscreen mode on the given monitor using the given resolution and refresh rate.
	void fullscreen_mode(glfw_wrapper::monitor& mon, glm::ivec2 resolution, int refresh_rate);
	/// Sets the window to windowed mode at the given position and with the given size.
	void windowed_mode(glm::ivec2 pos, glm::ivec2 size);
	/// Set the window to windowed fullscreen mode on the given monitor.
	void windowed_fullscreen_mode(glfw_wrapper::monitor& mon);

	/// Sets the callback function object to be called on key presses, releases and repetition.
	/**
	 * Requires f to be callable with signature
	 * <code>void(key key, int scancode, button_action button_action, modifier_flags mods)</code>.
	 */
	template <typename F>
	void key_callback(F&& f) {
		callbacks_->key = std::forward<F>(f);
	}
	/// Sets the callback function object to be called on character input.
	/**
	 * Requires f to be callable with signature
	 * <code>void(unsigned int codepoint)</code>.
	 */
	template <typename F>
	void character_callback(F&& f) {
		callbacks_->character = std::forward<F>(f);
	}
	/// Sets the callback function object to be called on character input, additionally capturing modifiers.
	/**
	 * Requires f to be callable with signature
	 * <code>void(unsigned int codepoint, modifier_flags mods)</code>.
	 */
	template <typename F>
	void character_mods_callback(F&& f) {
		callbacks_->charmods = std::forward<F>(f);
	}
	/// Sets the callback function object to be called on (mouse) cursor movement.
	/**
	 * Requires f to be callable with signature
	 * <code>void(double xpos, double ypos)</code>.
	 */
	template <typename F>
	void cursor_position_callback(F&& f) {
		callbacks_->cursor_position = std::forward<F>(f);
	}
	/// Sets the callback function object to be called on the cursor entering or leaving the window.
	/**
	 * Requires f to be callable with signature
	 * <code>void(bool entered)</code>.
	 */
	template <typename F>
	void cursor_enter_callback(F&& f) {
		callbacks_->cursor_enter = std::forward<F>(f);
	}
	/// Sets the callback function object to be called on mouse button presses, releases and repetitions.
	/**
	 * Requires f to be callable with signature
	 * <code>void(mouse_button button, button_action button_action, modifier_flags mods)</code>.
	 */
	template <typename F>
	void mouse_button_callback(F&& f) {
		callbacks_->mouse_button = std::forward<F>(f);
	}
	/// Sets the callback function object to be called on scroll wheel input.
	/**
	 * Requires f to be callable with signature
	 * <code>void(double xoffset, double yoffset)</code>.
	 */
	template <typename F>
	void scroll_callback(F&& f) {
		callbacks_->scroll = std::forward<F>(f);
	}
	/// Sets the callback function object to be called when closing of the window was requested.
	/**
	 * Requires f to be callable with signature
	 * <code>void()</code>.
	 */
	template <typename F>
	void window_close_callback(F&& f) {
		callbacks_->window_close = std::forward<F>(f);
	}
	/// Sets the callback function object to be called when the window was resized.
	/**
	 * Requires f to be callable with signature
	 * <code>void(int width, int height)</code>.
	 *
	 * The window size is reported in virtual screen coordinates.
	 */
	template <typename F>
	void window_size_callback(F&& f) {
		callbacks_->window_size = std::forward<F>(f);
	}
	/// \brief Sets the callback function object to be called when the window (and therefore the associated
	/// framebuffer) was resized.
	/**
	 * Requires f to be callable with signature
	 * <code>void(int width, int height)</code>.
	 *
	 * The framebuffer size is reported in pixels.
	 */
	template <typename F>
	void framebuffer_size_callback(F&& f) {
		callbacks_->framebuffer_size = std::forward<F>(f);
	}
	/// Sets the callback function object to be called when the window position was changed.
	/**
	 * Requires f to be callable with signature
	 * <code>void(int xpos, int ypos)</code>.
	 */
	template <typename F>
	void window_position_callback(F&& f) {
		callbacks_->window_pos = std::forward<F>(f);
	}
	/// Sets the callback function object to be called when the window was iconified or restored.
	/**
	 * Requires f to be callable with signature
	 * <code>void(bool iconified)</code>.
	 */
	template <typename F>
	void window_iconify_callback(F&& f) {
		callbacks_->window_iconify = std::forward<F>(f);
	}
	/// Sets the callback function object to be called when the window received or lost focus.
	/**
	 * Requires f to be callable with signature
	 * <code>void(bool focused)</code>.
	 */
	template <typename F>
	void window_focus_callback(F&& f) {
		callbacks_->window_focus = std::forward<F>(f);
	}
	/// \brief Sets the callback function object to be called when the window needs redrawing (triggered by
	/// the window system).
	/**
	 * Requires f to be callable with signature
	 * <code>void()</code>.
	 */
	template <typename F>
	void window_refresh_callback(F&& f) {
		callbacks_->window_refresh = std::forward<F>(f);
	}

	/// Sets the callback function object to be called when files are dragged and dropped onto the window.
	/**
	 * Requires f to be callable with signature
	 * <code>void(const std::vector<std::string>& paths)</code>.
	 */
	template <typename F>
	void window_drop_callback(F&& f) {
		callbacks_->window_drop = std::forward<F>(f);
	}
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_WINDOW_HPP_ */
