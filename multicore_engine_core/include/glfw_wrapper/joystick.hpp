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

/// Represents a joystick in glfw.
/**
 * Because of glfw limitations all member functions except id() may only be called on the main thread.
 */
class joystick {
	int id_;
	std::vector<float> axis_;
	std::vector<bool> buttons_;

	// cppcheck-suppress noExplicitConstructor
	joystick(int id) : id_{id} {}

	friend class instance;

public:
	/// Returns the id of the joystick.
	int id() {
		return id_;
	}
	/// Checks if the joystick is still present.
	bool present() const;
	/// Returns a vector with one float between -1.0 and 1.0 for each axis of the joystick.
	/**
	 * The referenced vector is valid until the next call or until the object is destroyed.
	 */
	const std::vector<float>& poll_axis();
	/// Returns the pressed state of the buttons of the joystick.
	/**
	 * The referenced vector is valid until the next call or until the object is destroyed.
	 */
	const std::vector<bool>& poll_buttons();
	/// Returns the name of the joystick device.
	std::string name() const;
};

} /* namespace glfw_wrapper */
} /* namespace mce */

#endif /* GLFW_WRAPPER_JOYSTICK_HPP_ */
