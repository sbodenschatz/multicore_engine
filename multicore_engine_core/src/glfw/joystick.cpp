/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/glfw/joystick.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/glfw/joystick.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>

namespace mce {
namespace glfw {

bool joystick::present() const {
	return glfwJoystickPresent(id_) == GLFW_TRUE;
}
const std::vector<float>& joystick::poll_axis() {
	int count = 0;
	auto a = glfwGetJoystickAxes(id_, &count);
	if(!a) {
		axis_.clear();
		return axis_;
	}
	axis_.resize(count);
	std::copy(a, a + count, axis_.begin());
	return axis_;
}
const std::vector<bool>& joystick::poll_buttons() {
	int count = 0;
	auto b = glfwGetJoystickButtons(id_, &count);
	if(!b) {
		buttons_.clear();
		return buttons_;
	}
	buttons_.resize(count);
	std::transform(b, b + count, buttons_.begin(), [](unsigned char c) { return c == GLFW_PRESS; });
	return buttons_;
}
std::string joystick::name() const {
	return glfwGetJoystickName(id_);
}

} /* namespace glfw_wrapper */
} /* namespace mce */
