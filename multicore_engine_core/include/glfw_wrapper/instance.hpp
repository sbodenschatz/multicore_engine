/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/instance.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_INSTANCE_HPP_
#define GLFW_WRAPPER_INSTANCE_HPP_

#include <boost/container/small_vector.hpp>
#include <functional>
#include <glfw_wrapper/glfw_defs.hpp>
#include <mutex>
#include <util/copy_on_write.hpp>

struct GLFWmonitor;

namespace mce {
namespace glfw_wrapper {
class monitor;
class joystick;

class instance {
	static std::mutex init_mutex;
	static size_t init_refcount;
	typedef size_t callback_id;
	static std::atomic<callback_id> next_callback_id;
	typedef boost::container::small_vector<std::pair<callback_id, std::function<void(int, const char*)>>,
										   0x100>
			error_callback_container;
	static util::copy_on_write<error_callback_container> error_callback_functions;
	std::mutex error_callback_id_mutex;
	boost::container::small_vector<callback_id, 0x100> error_callback_ids;

	typedef boost::container::small_vector<
			std::pair<callback_id,
					  std::function<void(const glfw_wrapper::monitor& monitor, monitor_event event)>>,
			0x100>
			monitor_callback_container;
	static util::copy_on_write<monitor_callback_container> monitor_callback_functions;
	std::mutex monitor_callback_id_mutex;
	boost::container::small_vector<callback_id, 0x100> monitor_callback_ids;

	static void error_callback(int error_code, const char* description);
	static void monitor_callback(GLFWmonitor* monitor, int event);

public:
	instance();
	~instance();
	instance(const instance&) = delete;
	instance& operator=(const instance&) = delete;

	template <typename F>
	callback_id add_error_callback(const F& f) {
		callback_id id = next_callback_id++;
		{
			std::lock_guard<std::mutex> lock(error_callback_id_mutex);
			error_callback_ids.push_back(id);
		}
		error_callback_functions.do_transaction(
				[&](error_callback_container& efc) { efc.emplace_back(id, f); });
		return id;
	}

	void remove_error_callback(callback_id id);

	template <typename F>
	callback_id add_monitor_callback(const F& f) {
		callback_id id = next_callback_id++;
		{
			std::lock_guard<std::mutex> lock(monitor_callback_id_mutex);
			monitor_callback_ids.push_back(id);
		}
		monitor_callback_functions.do_transaction(
				[&](monitor_callback_container& efc) { efc.emplace_back(id, f); });
		return id;
	}

	void remove_monitor_callback(callback_id id);

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
