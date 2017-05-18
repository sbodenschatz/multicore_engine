/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw_wrapper/instance.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <GLFW/glfw3.h>
#include <algorithm>
#include <glfw_wrapper/instance.hpp>

namespace mce {
namespace glfw_wrapper {

std::mutex instance::init_mutex;
size_t instance::init_refcount;
std::atomic<instance::callback_id> instance::next_callback_id{0};
util::copy_on_write<instance::error_callback_container> instance::error_callback_functions;

instance::instance() {
	std::lock_guard<std::mutex> lock(init_mutex);
	if(init_refcount == 0) {
		glfwInit();
		glfwSetErrorCallback(error_callback);
	}
	init_refcount++;
}

instance::~instance() {
	error_callback_functions.do_transaction([&](error_callback_container& efc) {
		efc.erase(std::remove_if(efc.begin(), efc.end(), [&](auto& e) {
			return std::find(error_callback_ids.begin(), error_callback_ids.end(), e.first) !=
				   error_callback_ids.end();
		}));
	});
	std::lock_guard<std::mutex> lock(init_mutex);
	init_refcount--;
	if(init_refcount == 0) {
		glfwTerminate();
	}
}

void instance::error_callback(int error_code, const char* description) {
	auto cb_container = error_callback_functions.get();
	for(auto& e : *cb_container) {
		e.second(error_code, description);
	}
}

void instance::monitor_callback(GLFWmonitor* monitor, int event) {
	auto cb_container = monitor_callback_functions.get();
	for(auto& e : *cb_container) {
		e.second(monitor, event);
	}
}

void instance::remove_error_callback(callback_id id) {
	error_callback_functions.do_transaction([&](error_callback_container& efc) {
		efc.erase(std::remove_if(efc.begin(), efc.end(), [&](auto& e) { return e.first == id; }));
	});
	std::lock_guard<std::mutex> lock(error_callback_id_mutex);
	error_callback_ids.erase(std::remove_if(error_callback_ids.begin(), error_callback_ids.end(),
											[&](auto& e) { return e == id; }));
}

void instance::remove_monitor_callback(callback_id id) {
	monitor_callback_functions.do_transaction([&](monitor_callback_container& efc) {
		efc.erase(std::remove_if(efc.begin(), efc.end(), [&](auto& e) { return e.first == id; }));
	});
	std::lock_guard<std::mutex> lock(monitor_callback_id_mutex);
	monitor_callback_ids.erase(std::remove_if(monitor_callback_ids.begin(), monitor_callback_ids.end(),
											  [&](auto& e) { return e == id; }));
}

void instance::poll_events() {
	glfwPollEvents();
}
void instance::wait_events() {
	glfwWaitEvents();
}
void instance::wait_events(double timeout) {
	glfwWaitEventsTimeout(timeout);
}
void instance::post_empty_event() {
	glfwPostEmptyEvent();
}
std::string instance::key_name(key key, int scancode) const {
	return glfwGetKeyName(static_cast<int>(key), scancode);
}

} // namespace glfw_wrapper
} // namespace mce
