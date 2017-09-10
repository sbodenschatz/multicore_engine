/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/input/input_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_INPUT_SYSTEM_HPP_
#define MCE_INPUT_INPUT_SYSTEM_HPP_

/**
 * \file
 * Defines the system class for the input system.
 */

#include <boost/container/flat_map.hpp>
#include <glm/vec2.hpp>
#include <mce/core/system.hpp>
#include <mce/glfw/glfw_defs.hpp>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace windowing {
class window_system;
} // namespace windowing
namespace input {

/// Encapsulates the state data related to the mouse.
struct mouse_state {
	glm::dvec2 position;	 ///< The position of the mouse in this state.
	glm::dvec2 velocity;	 ///< The velocity of the mouse in this state.
	glm::dvec2 acceleration; ///< The acceleration of the mouse in this state.
	/// Indicates for each glfw::mouse_button whether it is pressed in this state.
	bool buttons[size_t(glfw::mouse_button::last) + 1] = {};
};

/// \brief Implements a system that handles the logic for input processing (in combination with input_state
/// and associated component classes).
class input_system : public core::system {
	core::engine& eng;
	windowing::window_system& win_sys;
	boost::container::flat_map<glfw::key, bool> current_key_state_;
	boost::container::flat_map<glfw::key, bool> last_key_state_;
	mouse_state current_mouse_state_;
	mouse_state last_mouse_state_;

	friend class input_state;

public:
	/// Returns the phase ordering index for pre hooks for this system.
	int pre_phase_ordering() const noexcept override {
		return 0x1100;
	}
	/// Returns the phase ordering index for post hooks for this system.
	int post_phase_ordering() const noexcept override {
		return 0x1100;
	}

	/// Constructs the input system for the given engine and the given window_system required as a dependency.
	input_system(core::engine& eng, windowing::window_system& win_sys);
	/// Destroys the input_system and the associated resources.
	~input_system();

	/// Hook function to obtain current key and mouse state at the start of the frame.
	void preprocess(const mce::core::frame_time& frame_time) override;

	/// Returns a bool indicating whether the given glfw::key is pressed in the current frame.
	bool current_key_state(glfw::key k) const noexcept {
		auto it = current_key_state_.find(k);
		if(it != current_key_state_.end()) return it->second;
		return false;
	}

	/// Returns the mouse_state for the current frame.
	const mouse_state& current_mouse_state() const noexcept {
		return current_mouse_state_;
	}

	/// Returns a bool indicating whether the given glfw::key was pressed in the last frame.
	bool last_key_state(glfw::key k) const noexcept {
		auto it = last_key_state_.find(k);
		if(it != last_key_state_.end()) return it->second;
		return false;
	}

	/// Returns the mouse_state for the current frame.
	const mouse_state& last_mouse_state() const noexcept {
		return last_mouse_state_;
	}
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_INPUT_SYSTEM_HPP_ */
