/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/instance.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_INSTANCE_HPP_
#define GLFW_WRAPPER_INSTANCE_HPP_

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

class instance {
	static std::mutex init_mutex;
	static size_t init_refcount;
	typedef size_t callback_id;
	detail::observable<error_code, boost::string_view> error_callbacks;
	detail::observable<const glfw_wrapper::monitor&, event> monitor_callbacks;
	detail::observable<const glfw_wrapper::joystick&, event> joystick_callbacks;

	static void error_callback(int error, const char* description);
	static void monitor_callback(GLFWmonitor* m, int event);
	static void joystick_callback(int joy, int event);

public:
	instance();
	~instance();
	instance(const instance&) = delete;
	instance& operator=(const instance&) = delete;

	template <typename F>
	callback_id add_error_callback(const F& f) {
		return error_callbacks.add_callback(f);
	}

	void remove_error_callback(callback_id id) {
		error_callbacks.remove_callback(id);
	}

	template <typename F>
	callback_id add_monitor_callback(const F& f) {
		return monitor_callbacks.add_callback(f);
	}

	void remove_monitor_callback(callback_id id) {
		monitor_callbacks.remove_callback(id);
	}

	template <typename F>
	callback_id add_joystick_callback(const F& f) {
		return joystick_callbacks.add_callback(f);
	}

	void remove_joystick_callback(callback_id id) {
		joystick_callbacks.remove_callback(id);
	}

	void poll_events();
	void wait_events();
	void wait_events(double timeout);
	void post_empty_event();
	std::string key_name(key key, int scancode) const;
	std::vector<joystick> query_joysticks() const;
};

} // namespace glfw_wrapper
} // namespace mce

#endif /* GLFW_WRAPPER_INSTANCE_HPP_ */
