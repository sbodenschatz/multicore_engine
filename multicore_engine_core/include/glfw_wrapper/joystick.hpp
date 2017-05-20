/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/glfw_wrapper/joystick.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef GLFW_WRAPPER_JOYSTICK_HPP_
#define GLFW_WRAPPER_JOYSTICK_HPP_

#include <glfw_wrapper/glfw_defs.hpp>
#include <string>
#include <vector>

namespace mce {
namespace glfw_wrapper {

class joystick {
	int id_;
	std::vector<float> axis_;
	std::vector<button_action> buttons_;

	// cppcheck-suppress noExplicitConstructor
	joystick(int id) : id_{id} {}

	friend class instance;

public:
	int id() {
		return id_;
	}
	bool present() const;
	const std::vector<float>& poll_axis();
	const std::vector<button_action>& poll_buttons();
	std::string name() const;
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_JOYSTICK_HPP_ */
