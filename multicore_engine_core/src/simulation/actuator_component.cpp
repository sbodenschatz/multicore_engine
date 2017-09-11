/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_component.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/simulation/actuator_component.hpp>

namespace mce {
namespace simulation {

actuator_component::actuator_component(entity::entity& owner,
									   const entity::component_configuration& configuration,
									   actuator_state& state)
		: component(owner, configuration), state_{state} {}

actuator_component::~actuator_component() {}

void actuator_component::process(const mce::core::frame_time& frame_time) {
	if(pattern_) {
		pattern_(frame_time, owner());
	}
}

} /* namespace simulation */
} /* namespace mce */
