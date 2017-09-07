/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/input/input_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_INPUT_STATE_HPP_
#define MCE_INPUT_INPUT_STATE_HPP_

#include <mce/containers/smart_object_pool.hpp>
#include <mce/containers/smart_pool_ptr.hpp>
#include <mce/core/system_state.hpp>
#include <mce/input/first_person_flyer_component.hpp>

namespace mce {
namespace input {

class input_state : public core::system_state {
	containers::smart_object_pool<first_person_flyer_component> first_person_flyer_comps;

public:
	explicit input_state(mce::core::system* system);
	~input_state();

	/// Creates a first_person_flyer_component for the given entity and using the given configuration.
	containers::smart_pool_ptr<first_person_flyer_component>
	create_first_person_flyer_component(entity::entity& owner,
										const entity::component_configuration& configuration) {
		return first_person_flyer_comps.emplace(owner, configuration);
	}

	/// Registers the component types managed by input_state to the given entity_manager object.
	void register_to_entity_manager(entity::entity_manager& em);
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_INPUT_STATE_HPP_ */
