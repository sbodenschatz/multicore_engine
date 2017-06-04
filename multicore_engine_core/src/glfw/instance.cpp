/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/glfw_wrapper/instance.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <GLFW/glfw3.h>
#include <algorithm>
#include <mce/glfw/instance.hpp>
#include <mce/glfw/joystick.hpp>
#include <mce/glfw/monitor.hpp>

namespace mce {
namespace glfw {

std::mutex instance::init_mutex;
size_t instance::init_refcount;

instance::instance() {
	std::lock_guard<std::mutex> lock(init_mutex);
	if(init_refcount == 0) {
		glfwInit();
		glfwSetErrorCallback(error_callback);
		glfwSetMonitorCallback(monitor_callback);
	}
	init_refcount++;
}

instance::~instance() {
	std::lock_guard<std::mutex> lock(init_mutex);
	init_refcount--;
	if(init_refcount == 0) {
		glfwTerminate();
	}
}

void instance::error_callback(int error, const char* description) {
	decltype(error_callbacks)::callback_s(static_cast<error_code>(error), description);
}
void instance::monitor_callback(GLFWmonitor* m, int event) {
	decltype(monitor_callbacks)::callback_s(m, static_cast<glfw::device_event>(event));
}
void instance::joystick_callback(int joy, int event) {
	decltype(joystick_callbacks)::callback_s(joy, static_cast<glfw::device_event>(event));
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
std::vector<joystick> instance::query_joysticks() const {
	std::vector<joystick> joysticks;
	joysticks.reserve(GLFW_JOYSTICK_LAST + 1);
	for(int id = GLFW_JOYSTICK_1; id < GLFW_JOYSTICK_LAST + 1; ++id) {
		if(glfwJoystickPresent(id) == GLFW_TRUE) {
			joysticks.push_back(id);
		}
	}
	return joysticks;
}

bool instance::vulkan_supported() const {
	return glfwVulkanSupported() == GLFW_TRUE;
}
std::vector<std::string> instance::required_vulkan_instance_extensions() const {
	std::vector<std::string> res;
	uint32_t count = 0;
	auto e = glfwGetRequiredInstanceExtensions(&count);
	if(!e) {
		res.clear();
		return res;
	}
	res.reserve(count);
	std::copy(e, e + count, std::back_inserter(res));
	return res;
}

} // namespace glfw_wrapper
} // namespace mce
