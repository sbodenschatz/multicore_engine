/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_SIMULATION_ACTUATOR_COMPONENT_HPP_
#define MCE_SIMULATION_ACTUATOR_COMPONENT_HPP_

#include <mce/entity/component.hpp>
#include <mce/simulation/simulation_defs.hpp>

namespace mce {
namespace simulation {
class actuator_state;

class actuator_component : public entity::component {
	actuator_state& state_;
	movement_pattern_function pattern_;

public:
	actuator_component(entity::entity& owner, const entity::component_configuration& configuration,
					   actuator_state& state);
	~actuator_component();
};

} /* namespace simulation */
} /* namespace mce */

#endif /* MCE_SIMULATION_ACTUATOR_COMPONENT_HPP_ */
