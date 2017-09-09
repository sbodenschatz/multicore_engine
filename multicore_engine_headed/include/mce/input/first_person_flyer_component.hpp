/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/input/first_person_flyer_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_FIRST_PERSON_FLYER_COMPONENT_HPP_
#define MCE_INPUT_FIRST_PERSON_FLYER_COMPONENT_HPP_

#include <mce/entity/component.hpp>
#include <mce/glfw/glfw_defs.hpp>
#include <mce/glfw/name_mapping.hpp>

namespace mce {
namespace input {
class input_system;

class first_person_flyer_component : public entity::component {
	glfw::key forward_key_ = glfw::key::k_w;
	glfw::key backward_key_ = glfw::key::k_s;
	glfw::key left_key_ = glfw::key::k_a;
	glfw::key right_key_ = glfw::key::k_d;
	glfw::key upward_key_ = glfw::key::k_r;
	glfw::key downward_key_ = glfw::key::k_f;
	float speed_ = 10.0f;
	float rotation_speed_ = 10.0f;
	bool inverted_y_axis_ = false;

	void process_keyboard(const mce::core::frame_time& frame_time, const input_system& sys);
	void process_mouse(const mce::core::frame_time& frame_time, const input_system& sys);
	void enforce_no_roll();

	void process(const mce::core::frame_time& frame_time, const input_system& sys);

	friend class input_state;

public:
	first_person_flyer_component(entity::entity& owner, const entity::component_configuration& configuration);
	~first_person_flyer_component();

	std::string backward_key_name() const {
		return glfw::to_string(backward_key_);
	}

	void backward_key_name(const std::string& backward_key_name) {
		backward_key_ = glfw::key_from_string(backward_key_name);
	}

	std::string downward_key_name() const {
		return glfw::to_string(downward_key_);
	}

	void downward_key_name(const std::string& downward_key_name) {
		downward_key_ = glfw::key_from_string(downward_key_name);
	}

	std::string forward_key_name() const {
		return glfw::to_string(forward_key_);
	}

	void forward_key_name(const std::string& forward_key_name) {
		forward_key_ = glfw::key_from_string(forward_key_name);
	}

	std::string left_key_name() const {
		return glfw::to_string(left_key_);
	}

	void left_key_name(const std::string& left_key_name) {
		left_key_ = glfw::key_from_string(left_key_name);
	}

	std::string right_key_name() const {
		return glfw::to_string(right_key_);
	}

	void right_key_name(const std::string& right_key_name) {
		right_key_ = glfw::key_from_string(right_key_name);
	}

	std::string upward_key_name() const {
		return glfw::to_string(upward_key_);
	}

	void upward_key_name(const std::string& upward_key_name) {
		upward_key_ = glfw::key_from_string(upward_key_name);
	}

	glfw::key backward_key() const {
		return backward_key_;
	}

	void backward_key(glfw::key backward_key) {
		backward_key_ = backward_key;
	}

	glfw::key downward_key() const {
		return downward_key_;
	}

	void downward_key(glfw::key downward_key) {
		downward_key_ = downward_key;
	}

	glfw::key forward_key() const {
		return forward_key_;
	}

	void forward_key(glfw::key forward_key) {
		forward_key_ = forward_key;
	}

	glfw::key left_key() const {
		return left_key_;
	}

	void left_key(glfw::key left_key) {
		left_key_ = left_key;
	}

	glfw::key right_key() const {
		return right_key_;
	}

	void right_key(glfw::key right_key) {
		right_key_ = right_key;
	}

	glfw::key upward_key() const {
		return upward_key_;
	}

	void upward_key(glfw::key upward_key) {
		upward_key_ = upward_key;
	}

	float speed() const {
		return speed_;
	}

	void speed(float speed) {
		speed_ = speed;
	}

	bool inverted_y_axis() const {
		return inverted_y_axis_;
	}

	void inverted_y_axis(bool inverted_y_axis) {
		inverted_y_axis_ = inverted_y_axis;
	}

	float rotation_speed() const {
		return rotation_speed_;
	}

	void rotation_speed(float rotation_speed) {
		rotation_speed_ = rotation_speed;
	}

	/// Fills the given property_list with the properties available for this class.
	static void fill_property_list(property_list& prop);
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_FIRST_PERSON_FLYER_COMPONENT_HPP_ */
