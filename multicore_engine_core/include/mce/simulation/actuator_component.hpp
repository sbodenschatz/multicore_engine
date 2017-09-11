/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_SIMULATION_ACTUATOR_COMPONENT_HPP_
#define MCE_SIMULATION_ACTUATOR_COMPONENT_HPP_

/**
 * \file
 * Defines the actuator_component class.
 */

#include <mce/entity/component.hpp>
#include <mce/simulation/simulation_defs.hpp>

namespace mce {
namespace simulation {
class actuator_state;

/// Allows moving the entity it is attached to using a movement_pattern_function defined in actuator_system.
class actuator_component : public entity::component {
	actuator_state& state_;
	movement_pattern_function movement_pattern_;
	std::string movement_pattern_name_;

public:
	/// Creates an actuator_component for the given owner, configuration and actuator_state.
	actuator_component(entity::entity& owner, const entity::component_configuration& configuration,
					   actuator_state& state);
	/// Destroyes the actuator_component.
	~actuator_component();

	/// Performs the per-frame processing for this component by calling the movement_pattern_function if it is
	/// set to a valid function object.
	void process(const mce::core::frame_time& ft);

	/// Allows read-only access to the movement_pattern_function object.
	const movement_pattern_function& movement_pattern() const {
		return movement_pattern_;
	}

	/// Returns the name of the currently set movement_pattern_function object.
	std::string movement_pattern_name() const {
		return movement_pattern_name_;
	}

	/// \brief Sets the movement_pattern_function object to the registered object with the given name in the
	/// actuator_system.
	/**
	 * The object can be reset to empty by supplying an empty name string.
	 */
	void movement_pattern_name(const std::string& name);

	/// Fills the given property_list with the properties available for this class.
	static void fill_property_list(property_list& prop);
};

} /* namespace simulation */
} /* namespace mce */

#endif /* MCE_SIMULATION_ACTUATOR_COMPONENT_HPP_ */
