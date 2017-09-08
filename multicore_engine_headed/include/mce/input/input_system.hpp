/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/input/input_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_INPUT_SYSTEM_HPP_
#define MCE_INPUT_INPUT_SYSTEM_HPP_

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

struct mouse_state {
	glm::dvec2 position;
	glm::dvec2 velocity;
	glm::dvec2 acceleration;
	bool buttons[size_t(glfw::mouse_button::last) + 1] = {};
};

class input_system : public core::system {
	core::engine& eng;
	windowing::window_system& win_sys;
	boost::container::flat_map<glfw::key, bool> current_key_state_;
	boost::container::flat_map<glfw::key, bool> last_key_state_;
	mouse_state current_mouse_state_;
	mouse_state last_mouse_state_;

public:
	/// Returns the phase ordering index for pre hooks for this system.
	int pre_phase_ordering() const noexcept override {
		return 0x1100;
	}
	/// Returns the phase ordering index for post hooks for this system.
	int post_phase_ordering() const noexcept override {
		return 0x1100;
	}

	input_system(core::engine& eng, windowing::window_system& win_sys);
	~input_system();

	void preprocess(const mce::core::frame_time& frame_time) override;

	bool current_key_state(glfw::key k) const noexcept {
		auto it = current_key_state_.find(k);
		if(it != current_key_state_.end()) return it->second;
		return false;
	}

	const mouse_state& current_mouse_state() const noexcept {
		return current_mouse_state_;
	}

	bool last_key_state(glfw::key k) const noexcept {
		auto it = last_key_state_.find(k);
		if(it != last_key_state_.end()) return it->second;
		return false;
	}

	const mouse_state& last_mouse_state() const noexcept {
		return last_mouse_state_;
	}
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_INPUT_SYSTEM_HPP_ */
