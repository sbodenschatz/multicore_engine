/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/src/input/input_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/entity/entity_manager.hpp>
#include <mce/input/input_state.hpp>

namespace mce {
namespace input {

input_state::input_state(mce::core::system* system) : system_state(system) {}

input_state::~input_state() {}

void input_state::register_to_entity_manager(entity::entity_manager& em) {
	REGISTER_COMPONENT_TYPE_SIMPLE(em, first_person_flyer,
								   this->create_first_person_flyer_component(owner, config), this);
}

} /* namespace input */
} /* namespace mce */
