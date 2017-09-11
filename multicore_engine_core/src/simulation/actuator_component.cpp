/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_component.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/simulation/actuator_component.hpp>
#include <mce/simulation/actuator_state.hpp>
#include <mce/simulation/actuator_system.hpp>

namespace mce {
namespace simulation {

actuator_component::actuator_component(entity::entity& owner,
									   const entity::component_configuration& configuration,
									   actuator_state& state)
		: component(owner, configuration), state_{state} {}

actuator_component::~actuator_component() {}

void actuator_component::process(const mce::core::frame_time& frame_time) {
	if(movement_pattern_) {
		movement_pattern_(frame_time, owner());
	}
}

void actuator_component::fill_property_list(property_list& prop) {
	REGISTER_COMPONENT_PROPERTY_NAME_PROXY(prop, actuator_component, std::string, movement_pattern);
}

void actuator_component::movement_pattern_name(const std::string& name) {
	if(name.empty()) {
		movement_pattern_ = {};
	} else {
		movement_pattern_ = static_cast<const actuator_system*>(state_.system_)
									->find_movement_pattern(movement_pattern_name_);
	}
	movement_pattern_name_ = name;
}

} /* namespace simulation */
} /* namespace mce */
