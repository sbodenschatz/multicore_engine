/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/joystick.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <GLFW/glfw3.h>
#include <algorithm>
#include <glfw_wrapper/joystick.hpp>

namespace mce {
namespace glfw_wrapper {

bool joystick::present() const {
	return glfwJoystickPresent(id_) == GLFW_TRUE;
}
const std::vector<float>& joystick::poll_axis() {
	int count = 0;
	auto a = glfwGetJoystickAxes(id_, &count);
	axis_.resize(count);
	std::copy(a, a + count, axis_.begin());
	return axis_;
}
const std::vector<button_action>& joystick::poll_buttons() {
	int count = 0;
	auto b = glfwGetJoystickButtons(id_, &count);
	buttons_.resize(count);
	std::transform(b, b + count, buttons_.begin(), [](unsigned char c) { return static_cast<button_action>(c); });
	return buttons_;
}
std::string joystick::name() const {
	return glfwGetJoystickName(id_);
}

} /* namespace glfw_wrapper */
} /* namespace mce */
