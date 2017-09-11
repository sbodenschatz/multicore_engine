/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_system.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_SIMULATION_ACTUATOR_SYSTEM_HPP_
#define MCE_SIMULATION_ACTUATOR_SYSTEM_HPP_

#include <mce/core/system.hpp>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace simulation {

class actuator_system : public core::system {
	core::engine& eng_;

public:
	actuator_system(core::engine& eng);
	~actuator_system();
};

} /* namespace simulation */
} /* namespace mce */

#endif /* MCE_SIMULATION_ACTUATOR_SYSTEM_HPP_ */
