/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/input/input_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_INPUT_SYSTEM_HPP_
#define MCE_INPUT_INPUT_SYSTEM_HPP_

#include <mce/core/system.hpp>

namespace mce {
namespace core {
class engine;
class window_system;
} // namespace core
namespace input {

class input_system : public core::system {
	core::engine& eng;
	core::window_system& win_sys;

public:
	input_system(core::engine& eng, core::window_system& win_sys);
	~input_system();
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_INPUT_SYSTEM_HPP_ */
