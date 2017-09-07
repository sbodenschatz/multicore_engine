/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/input/input_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_INPUT_STATE_HPP_
#define MCE_INPUT_INPUT_STATE_HPP_

#include <mce/core/system_state.hpp>

namespace mce {
namespace input {

class input_state : public core::system_state {
public:
	input_state(mce::core::system* system);
	~input_state();
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_INPUT_STATE_HPP_ */
