/*
 * Multi-Core Engine project
 * File /multicore_engine_headed/include/mce/input/first_person_input_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_INPUT_FIRST_PERSON_INPUT_STATE_HPP_
#define MCE_INPUT_FIRST_PERSON_INPUT_STATE_HPP_

/**
 * Defines the system_state class for the input system.
 */

#include <mce/containers/simple_smart_object_pool.hpp>
#include <mce/containers/smart_object_pool.hpp>
#include <mce/containers/smart_pool_ptr.hpp>
#include <mce/core/system_state.hpp>
#include <mce/entity/ecs_types.hpp>
#include <mce/input/first_person_flyer_component.hpp>

namespace mce {
namespace core {
class game_state;
} // namespace core
namespace input {
class input_system;

/// Adds input logic to control entities using first-person views to a game_state.
/**
 * This state disables the mouse cursor and switches the mouse to use virtual coordinates to allow view
 * rotation by moving the mouse.
 */
class first_person_input_state : public core::system_state {
	entity::component_pool<first_person_flyer_component, 4> first_person_flyer_comps;

public:
	/// Defines the type of system that should be injected by add_system_state.
	using owner_system = input_system;

	ALIGNED_NEW_AND_DELETE(first_person_input_state)

	/// Creates an first_person_input_state for the given input_system and the given game_state.
	/**
	 * Should be called through add_system_state, which will also automatically inject the input_system and
	 * game_state.
	 */
	explicit first_person_input_state(input_system* system, core::game_state*);
	/// Destroys the first_person_input_state.
	~first_person_input_state();

	/// Creates a first_person_flyer_component for the given entity and using the given configuration.
	entity::component_impl_pool_ptr<first_person_flyer_component>
	create_first_person_flyer_component(entity::entity& owner,
										const entity::component_configuration& configuration) {
		return first_person_flyer_comps.emplace(owner, configuration);
	}

	/// \brief Hook function that is called when the state owning this first_person_input_state is reentered
	/// to restore the correct glfw::cursor_mode needed for the input logic.
	void reenter(const boost::any& parameter) override;

	/// Hook function to perform the per-frame processing for the input components in this state.
	void process(const mce::core::frame_time& frame_time) override;

	/// Registers the component types managed by first_person_input_state to the given entity_manager object.
	void register_to_entity_manager(entity::entity_manager& em);
};

} /* namespace input */
} /* namespace mce */

#endif /* MCE_INPUT_FIRST_PERSON_INPUT_STATE_HPP_ */
