/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_SIMULATION_ACTUATOR_COMPONENT_HPP_
#define MCE_SIMULATION_ACTUATOR_COMPONENT_HPP_

#include <mce/entity/component.hpp>
#include <mce/simulation/simulation_defs.hpp>

namespace mce {
namespace simulation {
class actuator_state;

class actuator_component : public entity::component {
	actuator_state& state_;
	movement_pattern_function movement_pattern_;
	std::string movement_pattern_name_;

public:
	actuator_component(entity::entity& owner, const entity::component_configuration& configuration,
					   actuator_state& state);
	~actuator_component();

	void process(const mce::core::frame_time& ft);

	const movement_pattern_function& movement_pattern() const {
		return movement_pattern_;
	}

	std::string movement_pattern_name() const {
		return movement_pattern_name_;
	}

	void movement_pattern_name(const std::string& name);

	/// Fills the given property_list with the properties available for this class.
	static void fill_property_list(property_list& prop);
};

} /* namespace simulation */
} /* namespace mce */

#endif /* MCE_SIMULATION_ACTUATOR_COMPONENT_HPP_ */
