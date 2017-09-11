/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/simulation/actuator_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_SIMULATION_ACTUATOR_STATE_HPP_
#define MCE_SIMULATION_ACTUATOR_STATE_HPP_

/**
 * \file
 * Defines the actuator_state class.
 */

#include <mce/containers/smart_object_pool.hpp>
#include <mce/core/system_state.hpp>
#include <mce/simulation/actuator_component.hpp>
#include <mce/simulation/actuator_system.hpp>

namespace mce {
namespace core {
class system;
class game_state;
} // namespace core
namespace entity {
class entity_manager;
} // namespace entity
namespace simulation {
class actuator_system;

/// \brief Adds actuator logic to a game_state allowing to control the movement of entities with
/// actuator_component objects attached by movement patterns defined in actuator_system.
class actuator_state : public core::system_state {
	containers::smart_object_pool<actuator_component> actuator_comps;

	friend class actuator_component;

public:
	/// Defines the type of system that should be injected by add_system_state.
	using owner_system = actuator_system;

	ALIGNED_NEW_AND_DELETE(actuator_state)

	/// Creates a actuator_state for the given system and game_state.
	/**
	 * Should be called game_state::add_system_state that injects the parameters.
	 */
	actuator_state(core::system* system, core::game_state*);
	/// Destroys the actuator_state.
	~actuator_state();

	/// Creates an actuator_component for the given entity and using the given configuration.
	containers::smart_pool_ptr<actuator_component>
	create_actuator_component(entity::entity& owner, const entity::component_configuration& configuration) {
		return actuator_comps.emplace(owner, configuration, *this);
	}

	/// Hook function called for the processing phase of a frame.
	void process(const mce::core::frame_time& frame_time) override;

	/// Registers the component types managed by input_state to the given entity_manager object.
	void register_to_entity_manager(entity::entity_manager& em);
};

} /* namespace simulation */
} /* namespace mce */

#endif /* MCE_SIMULATION_ACTUATOR_STATE_HPP_ */
