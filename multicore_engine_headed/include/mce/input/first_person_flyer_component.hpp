/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/input/first_person_flyer_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_FIRST_PERSON_FLYER_COMPONENT_HPP_
#define MCE_INPUT_FIRST_PERSON_FLYER_COMPONENT_HPP_

/**
 * \file
 * Defines the first_person_flyer_component class.
 */

#include <mce/entity/component.hpp>
#include <mce/glfw/glfw_defs.hpp>
#include <mce/glfw/name_mapping.hpp>

namespace mce {
namespace input {
class input_system;

/// \brief Provides input logic for flying around in the scene using a first person camera to the entity it is
/// attached to.
class first_person_flyer_component : public entity::component {
	std::string forward_key_ = "%forward%w";
	std::string backward_key_ = "%backward%s";
	std::string left_key_ = "%left%a";
	std::string right_key_ = "%right%d";
	std::string upward_key_ = "%upward%r";
	std::string downward_key_ = "%downward%f";
	float speed_ = 10.0f;
	float rotation_speed_ = 10.0f;
	bool inverted_y_axis_ = false;

	void process_keyboard(const mce::core::frame_time& frame_time, const input_system& sys);
	void process_mouse(const mce::core::frame_time& frame_time, const input_system& sys);
	void enforce_no_roll();

	void process(const mce::core::frame_time& frame_time, const input_system& sys);

	friend class first_person_input_state;

public:
	/// Creates a first_person_flyer_component for the given entity and using the given configuration.
	first_person_flyer_component(entity::entity& owner, const entity::component_configuration& configuration);
	/// Destroys the first_person_flyer_component.
	~first_person_flyer_component();

	/// Returns the name of the key used for moving backwards.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	std::string backward_key_name() const {
		return backward_key_;
	}

	/// Sets the key used for moving backwards by name.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	void backward_key_name(const std::string& backward_key_name) {
		backward_key_ = backward_key_name;
	}

	/// Returns the name of the key used for moving downwards.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	std::string downward_key_name() const {
		return downward_key_;
	}

	/// Sets the key used for moving downwards by name.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	void downward_key_name(const std::string& downward_key_name) {
		downward_key_ = downward_key_name;
	}

	/// Returns the name of the key used for moving forwards.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	std::string forward_key_name() const {
		return forward_key_;
	}

	/// Sets the key used for moving forwards by name.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	void forward_key_name(const std::string& forward_key_name) {
		forward_key_ = forward_key_name;
	}

	/// Returns the name of the key used for moving left.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	std::string left_key_name() const {
		return left_key_;
	}

	/// Sets the key used for moving left by name.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	void left_key_name(const std::string& left_key_name) {
		left_key_ = left_key_name;
	}

	/// Returns the name of the key used for moving right.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	std::string right_key_name() const {
		return right_key_;
	}

	/// Sets the key used for moving right by name.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	void right_key_name(const std::string& right_key_name) {
		right_key_ = right_key_name;
	}

	/// Returns the name of the key used for moving upwards.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	std::string upward_key_name() const {
		return upward_key_;
	}

	/// Sets the key used for moving upwards by name.
	/**
	 * A % character prefix indicates that the key mapping is resolved through the config system, e.g.
	 * the name "%name" means that the value of the config variable "input.key.name" will be used to get the
	 * key name.
	 * A default value can be given after a second % character.
	 */
	void upward_key_name(const std::string& upward_key_name) {
		upward_key_ = upward_key_name;
	}

	/// Returns the movement speed factor.
	float speed() const {
		return speed_;
	}

	/// Sets the movement speed factor.
	void speed(float speed) {
		speed_ = speed;
	}

	/// Returns the inversion flag for the y axis movement.
	bool inverted_y_axis() const {
		return inverted_y_axis_;
	}

	/// Sets the inversion flag for the y axis movement.
	void inverted_y_axis(bool inverted_y_axis) {
		inverted_y_axis_ = inverted_y_axis;
	}

	/// Returns the rotation speed factor.
	float rotation_speed() const {
		return rotation_speed_;
	}

	/// Sets the rotation speed factor.
	void rotation_speed(float rotation_speed) {
		rotation_speed_ = rotation_speed;
	}

	/// Fills the given property_list with the properties available for this class.
	static void fill_property_list(property_list& prop);
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_FIRST_PERSON_FLYER_COMPONENT_HPP_ */
