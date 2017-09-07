/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/input/first_person_flyer_component.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/input/first_person_flyer_component.hpp>

namespace mce {
namespace input {

first_person_flyer_component::first_person_flyer_component(
		entity::entity& owner, const entity::component_configuration& configuration)
		: component(owner, configuration) {}

first_person_flyer_component::~first_person_flyer_component() {}

void first_person_flyer_component::fill_property_list(property_list& prop) {
	REGISTER_COMPONENT_PROPERTY_NAME_PROXY(prop, first_person_flyer_component, std::string, forward_key);
	REGISTER_COMPONENT_PROPERTY_NAME_PROXY(prop, first_person_flyer_component, std::string, backward_key);
	REGISTER_COMPONENT_PROPERTY_NAME_PROXY(prop, first_person_flyer_component, std::string, left_key);
	REGISTER_COMPONENT_PROPERTY_NAME_PROXY(prop, first_person_flyer_component, std::string, right_key);
	REGISTER_COMPONENT_PROPERTY_NAME_PROXY(prop, first_person_flyer_component, std::string, upward_key);
	REGISTER_COMPONENT_PROPERTY_NAME_PROXY(prop, first_person_flyer_component, std::string, downward_key);
}

} /* namespace input */
} /* namespace mce */
