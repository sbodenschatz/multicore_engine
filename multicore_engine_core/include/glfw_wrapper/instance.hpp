/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/instance.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_INSTANCE_HPP_
#define GLFW_WRAPPER_INSTANCE_HPP_

/**
 * \file
 * Defines the glfw wrapper instance class providing support for initialization, termination and global
 * functionality of glfw.
 */

#include <atomic>
#include <boost/container/small_vector.hpp>
#include <boost/utility/string_view.hpp>
#include <functional>
#include <glfw_wrapper/glfw_defs.hpp>
#include <mutex>
#include <util/copy_on_write.hpp>

struct GLFWmonitor;

namespace mce {
namespace glfw_wrapper {
class monitor;
class joystick;

namespace detail {

template <typename... Args>
class observable {
	typedef size_t callback_id;
	static std::atomic<callback_id> next_callback_id;
	typedef boost::container::small_vector<std::pair<callback_id, std::function<void(Args...)>>, 0x100>
			callback_container;
	static util::copy_on_write<callback_container> callback_functions;
	std::mutex callback_id_mutex;
	boost::container::small_vector<callback_id, 0x100> callback_ids;

public:
	static void callback_s(Args... args) {
		auto cb_container = callback_functions.get();
		for(auto& e : *cb_container) {
			e.second(std::forward<Args>(args)...);
		}
	}
	template <typename F>
	callback_id add_callback(const F& f) {
		callback_id id = next_callback_id++;
		{
			std::lock_guard<std::mutex> lock(callback_id_mutex);
			callback_ids.push_back(id);
		}
		callback_functions.do_transaction([&](callback_container& cc) { cc.emplace_back(id, f); });
		return id;
	}
	void remove_callback(callback_id id) {
		callback_functions.do_transaction([&](callback_container& cc) {
			cc.erase(std::remove_if(cc.begin(), cc.end(), [&](auto& e) { return e.first == id; }));
		});
		std::lock_guard<std::mutex> lock(callback_id_mutex);
		callback_ids.erase(
				std::remove_if(callback_ids.begin(), callback_ids.end(), [&](auto& e) { return e == id; }));
	}
	~observable() {
		callback_functions.do_transaction([&](callback_container& cc) {
			cc.erase(std::remove_if(cc.begin(), cc.end(), [&](auto& e) {
				return std::find(callback_ids.begin(), callback_ids.end(), e.first) != callback_ids.end();
			}));
		});
	}
};

template <typename... Args>
std::atomic<typename observable<Args...>::callback_id> observable<Args...>::next_callback_id{0};

template <typename... Args>
util::copy_on_write<typename observable<Args...>::callback_container> observable<Args...>::callback_functions;

} // namespace detail

/// Handles initialization and termination of the glfw library and manages global functionality of it.
/**
 * Instances are not actually behaviorally separate from each other because glfw only handles this
 * functionality globally. Creating the first instance initializes the glfw library. Instances with
 * overlapping lifetimes are behaviorally equivalent. Destruction of the last instances terminates the glfw
 * library.
 *
 * Global functionality is provided through instance instead of through global function to ensure that the
 * library is initialized.
 *
 * Members of this class that map to glfw functionality may only be called on the main thread, except for
 * post_empty_event(). Only the callback registration and deregistration member functions and
 * post_empty_event() may be called form other threads.
 */
class instance {
	static std::mutex init_mutex;
	static size_t init_refcount;
	typedef size_t callback_id;
	detail::observable<error_code, boost::string_view> error_callbacks;
	detail::observable<const glfw_wrapper::monitor&, device_event> monitor_callbacks;
	detail::observable<const glfw_wrapper::joystick&, device_event> joystick_callbacks;

	static void error_callback(int error, const char* description);
	static void monitor_callback(GLFWmonitor* m, int event);
	static void joystick_callback(int joy, int event);

public:
	/// Creates the instance.
	instance();
	/// Destroys the instance.
	~instance();
	/// Forbids copying.
	instance(const instance&) = delete;
	/// Forbids copying.
	instance& operator=(const instance&) = delete;

	/// Adds a callback function object to be called when glfw errors occur.
	/**
	 * The type F must be callable with the signature <code>void(error_code error, boost::string_view
	 * description)</code>. The returned id can be used to remove it again.
	 */
	template <typename F>
	callback_id add_error_callback(const F& f) {
		return error_callbacks.add_callback(f);
	}

	/// Removes the error callback whose registration returned the given id.
	void remove_error_callback(callback_id id) {
		error_callbacks.remove_callback(id);
	}

	/// Adds a callback function object to be called when monitors are connected or disconnected.
	/**
	 * The type F must be callable with the signature <code>void(const monitor& monitor, event event)</code>.
	 * The returned id can be used to remove it again.
	 */
	template <typename F>
	callback_id add_monitor_callback(const F& f) {
		return monitor_callbacks.add_callback(f);
	}

	/// Removes the monitor callback whose registration returned the given id.
	void remove_monitor_callback(callback_id id) {
		monitor_callbacks.remove_callback(id);
	}

	/// Adds a callback function object to be called when joysticks are connected or disconnected.
	/**
	 * The type F must be callable with the signature <code>void(const joystick& joystick, event
	 * event)</code>. The returned id can be used to remove it again.
	 */
	template <typename F>
	callback_id add_joystick_callback(const F& f) {
		return joystick_callbacks.add_callback(f);
	}

	/// Removes the joystick callback whose registration returned the given id.
	void remove_joystick_callback(callback_id id) {
		joystick_callbacks.remove_callback(id);
	}

	/// Processes pending events in a non-blocking way.
	void poll_events();
	/// Waits for an event and processes it.
	void wait_events();
	/// Waits for an event and processes it with the given timeout for waiting.
	void wait_events(double timeout);
	/// Wakes up the main thread if it is waiting for events by posting an empty event into the event queue.
	void post_empty_event();
	/// Returns a descriptive name of the given key or scancode, see doc for glfwGetKeyName for details.
	std::string key_name(key key, int scancode) const;
	/// Returns the present joysticks.
	std::vector<joystick> query_joysticks() const;
};

} // namespace glfw_wrapper
} // namespace mce

#endif /* GLFW_WRAPPER_INSTANCE_HPP_ */
