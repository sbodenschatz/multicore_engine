/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/src/input/first_person_flyer_component.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/core_defs.hpp>
#include <mce/entity/entity.hpp>
#include <mce/input/first_person_flyer_component.hpp>
#include <mce/input/input_system.hpp>

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
	REGISTER_COMPONENT_PROPERTY(prop, first_person_flyer_component, float, speed);
}

void first_person_flyer_component::process(const mce::core::frame_time& frame_time, const input_system& sys) {
	process_mouse(frame_time, sys);
	process_keyboard(frame_time, sys);
}
void first_person_flyer_component::process_keyboard(const mce::core::frame_time& frame_time,
													const input_system& sys) {
	glm::vec4 velocity;
	if(sys.current_key_state(forward_key_)) {
		velocity.z = -1.0f;
	}
	if(sys.current_key_state(backward_key_)) {
		velocity.z = 1.0f;
	}
	if(sys.current_key_state(left_key_)) {
		velocity.x = -1.0f;
	}
	if(sys.current_key_state(right_key_)) {
		velocity.x = 1.0f;
	}
	if(sys.current_key_state(upward_key_)) {
		velocity.y = 1.0f;
	}
	if(sys.current_key_state(downward_key_)) {
		velocity.y = -1.0f;
	}
	if(dot(velocity, velocity) > 0.0) {
		velocity = glm::normalize(velocity) * speed_;
	}
	owner().position(owner().position() + velocity * frame_time.delta_t);
}
void first_person_flyer_component::process_mouse(const mce::core::frame_time& frame_time,
												 const input_system& sys) {
	glm::vec2 mouse_velocity = sys.current_mouse_state().velocity;
	glm::vec2 velocity = glm::radians(-rotation_speed / 100.0f * mouse_velocity);
	if(inverted_y_axis_) {
		velocity.y *= -1.0f;
	}
	glm::vec3 global_angualar_velocity = {0.0f, velocity.x, 0.0f};
	glm::vec3 local_angualar_velocity = {velocity.y, 0.0f, 0.0f};
	glm::quat global_rot_quad = {0.0f, global_angualar_velocity};
	glm::quat local_rot_quad = {0.0f, local_angualar_velocity};
	glm::quat orientation_derivative = 0.5f * (global_rot_quad * owner().orientation());
	orientation_derivative += 0.5f * (owner().orientation() * local_rot_quad);
	owner().orientation(glm::normalize(owner().orientation() + orientation_derivative * frame_time.delta_t));
}

} /* namespace input */
} /* namespace mce */
