/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/point_light_component.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/rendering/point_light_component.hpp>

namespace mce {
namespace rendering {

point_light_component::point_light_component(entity::entity& owner,
											 const entity::component_configuration& conf)
		: component(owner, conf), color_(1.0f, 1.0f, 1.0f), radius_(1000.0f), brightness_(1000.0f) {}

point_light_component::~point_light_component() {}

void point_light_component::fill_property_list(property_list& prop) {
	REGISTER_COMPONENT_PROPERTY(prop, point_light_component, glm::vec3, color);
	REGISTER_COMPONENT_PROPERTY(prop, point_light_component, float, radius);
	REGISTER_COMPONENT_PROPERTY(prop, point_light_component, float, brightness);
}

} /* namespace rendering */
} /* namespace mce */
