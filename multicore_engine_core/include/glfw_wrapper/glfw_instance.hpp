/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/glfw_instance.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_GLFW_INSTANCE_HPP_
#define GLFW_WRAPPER_GLFW_INSTANCE_HPP_

#include <boost/container/small_vector.hpp>
#include <functional>
#include <mutex>
#include <util/copy_on_write.hpp>

namespace mce {
namespace glfw_wrapper {

class glfw_instance {
	static std::mutex init_mutex;
	static size_t init_refcount;
	typedef size_t error_function_id;
	typedef boost::container::small_vector<
			std::pair<error_function_id, std::function<void(int, const char*)>>, 0x100>
			error_function_container;
	static std::atomic<error_function_id> next_error_function_id;
	static util::copy_on_write<error_function_container> error_functions;

	std::mutex error_id_mutex;
	boost::container::small_vector<error_function_id, 0x100> error_ids;

	static void error_callback(int error_code, const char* description);

public:
	glfw_instance();
	~glfw_instance();
	glfw_instance(const glfw_instance&) = delete;
	glfw_instance& operator=(const glfw_instance&) = delete;

	template <typename F>
	error_function_id add_error_callback(const F& f) {
		error_function_id id = next_error_function_id++;
		{
			std::lock_guard<std::mutex> lock(error_id_mutex);
			error_ids.push_back(id);
		}
		error_functions.do_transaction([&](error_function_container& efc) { efc.emplace_back(id, f); });
		return id;
	}

	void remove_error_callback(error_function_id id);
};

} // namespace glfw_wrapper
} // namespace mce

#endif /* GLFW_WRAPPER_GLFW_INSTANCE_HPP_ */
