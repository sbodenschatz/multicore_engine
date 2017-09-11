/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_SIMULATION_ACTUATOR_STATE_HPP_
#define MCE_SIMULATION_ACTUATOR_STATE_HPP_

#include <mce/core/system_state.hpp>

namespace mce {
namespace core {
class system;
class game_state;
} // namespace core
namespace simulation {
class actuator_system;

class actuator_state : public core::system_state {
public:
	/// Defines the type of system that should be injected by add_system_state.
	using owner_system = actuator_system;

	actuator_state(core::system* system, core::game_state*);
	~actuator_state();
};

} /* namespace simulation */
} /* namespace mce */

#endif /* MCE_SIMULATION_ACTUATOR_STATE_HPP_ */
