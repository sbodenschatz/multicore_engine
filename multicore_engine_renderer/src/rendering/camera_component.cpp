/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/camera_component.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/rendering/camera_component.hpp>

namespace mce {
namespace rendering {

camera_component::camera_component(entity::entity& owner, const entity::component_configuration& conf)
		: component(owner, conf), fov_{glm::radians(90.0f)}, near_plane_{1.0f}, far_plane_{100.0f} {}

camera_component::~camera_component() {}

void camera_component::fill_property_list(property_list& prop) {
	REGISTER_COMPONENT_PROPERTY(prop, camera_component, float, fov);
	REGISTER_COMPONENT_PROPERTY(prop, camera_component, float, near_plane);
	REGISTER_COMPONENT_PROPERTY(prop, camera_component, float, far_plane);
}

} /* namespace rendering */
} /* namespace mce */
