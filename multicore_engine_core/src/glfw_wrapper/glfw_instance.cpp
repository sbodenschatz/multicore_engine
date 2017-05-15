/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw_wrapper/glfw_instance.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <GLFW/glfw3.h>
#include <algorithm>
#include <glfw_wrapper/glfw_instance.hpp>

namespace mce {
namespace glfw_wrapper {

glfw_instance::glfw_instance() {
	std::lock_guard<std::mutex> lock(init_mutex);
	if(init_refcount == 0) {
		glfwInit();
		glfwSetErrorCallback(error_callback);
	}
	init_refcount++;
}

glfw_instance::~glfw_instance() {
	error_functions.do_transaction([&](error_function_container& efc) {
		efc.erase(std::remove_if(efc.begin(), efc.end(), [&](auto& e) {
			return std::find(error_ids.begin(), error_ids.end(), e.first) != error_ids.end();
		}));
	});
	std::lock_guard<std::mutex> lock(init_mutex);
	init_refcount--;
	if(init_refcount == 0) {
		glfwTerminate();
	}
}

void glfw_instance::error_callback(int error_code, const char* description) {
	auto efc = error_functions.get();
	for(auto& e : *efc) {
		e.second(error_code, description);
	}
}

void glfw_instance::remove_error_callback(error_function_id id) {
	error_functions.do_transaction([&](error_function_container& efc) {
		efc.erase(std::remove_if(efc.begin(), efc.end(), [&](auto& e) { return e.first == id; }));
	});
	std::lock_guard<std::mutex> lock(error_id_mutex);
	error_ids.erase(std::remove_if(error_ids.begin(), error_ids.end(), [&](auto& e) { return e == id; }));
}

} // namespace glfw_wrapper
} // namespace mce
