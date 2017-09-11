/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/entity/entity_manager.hpp>
#include <mce/simulation/actuator_state.hpp>

namespace mce {
namespace simulation {

actuator_state::actuator_state(core::system* system, core::game_state*) : system_state(system) {}

actuator_state::~actuator_state() {}

void actuator_state::register_to_entity_manager(entity::entity_manager& em) {
	REGISTER_COMPONENT_TYPE_SIMPLE(em, actuator, this->create_actuator_component(owner, config), this);
}

} /* namespace simulation */
} /* namespace mce */
