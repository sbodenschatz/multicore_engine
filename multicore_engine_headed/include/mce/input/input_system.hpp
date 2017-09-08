/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/input/input_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_INPUT_SYSTEM_HPP_
#define MCE_INPUT_INPUT_SYSTEM_HPP_

#include <mce/core/system.hpp>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace windowing {
class window_system;
} // namespace windowing
namespace input {

class input_system : public core::system {
	core::engine& eng;
	windowing::window_system& win_sys;

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
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_INPUT_SYSTEM_HPP_ */
