/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/input/input_state.hpp
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
class input_system;

class input_state : public core::system_state {
	containers::smart_object_pool<first_person_flyer_component> first_person_flyer_comps;

public:
	using owner_system = input_system;

	ALIGNED_NEW_AND_DELETE(input_state)

	explicit input_state(input_system* system);
	~input_state();

	/// Creates a first_person_flyer_component for the given entity and using the given configuration.
	containers::smart_pool_ptr<first_person_flyer_component>
	create_first_person_flyer_component(entity::entity& owner,
										const entity::component_configuration& configuration) {
		return first_person_flyer_comps.emplace(owner, configuration);
	}

	void reenter(const boost::any& parameter) override;

	/// Registers the component types managed by input_state to the given entity_manager object.
	void register_to_entity_manager(entity::entity_manager& em);
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_INPUT_STATE_HPP_ */
