/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/core/entity_game_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_CORE_ENTITY_GAME_STATE_HPP_
#define MCE_CORE_ENTITY_GAME_STATE_HPP_

/**
 * \file
 * Defines the entity_game_state class.
 */

#include <mce/core/game_state.hpp>
#include <mce/entity/entity_manager.hpp>

namespace mce {
namespace core {

/// Provides a base class for game states where entities are used.
/**
 * This class extends the game_state base class with functionality to automatically register component types
 * of added system states with the entity_manager also provided by this class.
 * For this automatic registration this class requires added systems to provide a member function
 * <code>void register_to_entity_manager(entity::entity_manager&)</code>.
 */
class entity_game_state : public game_state {
	entity::entity_manager entity_manager_;

protected:
	/// \brief Adds a system_state implemented by T to the game_state and registers the component types of T
	/// with the entity_manager of the game_state.
	/**
	 * T must provide a member function <code>void register_to_entity_manager(entity::entity_manager&)</code>.
	 */
	template <typename T, typename... Args>
	T* add_system_state(Args&&... args) {
		auto s = game_state::add_system_state<T>(std::forward<Args>(args)...);
		s->register_to_entity_manager(entity_manager_);
		return s;
	}

public:
	/// Constructs the entity_game_state with the given engine, state machine and parent state.
	entity_game_state(mce::core::engine* engine, mce::core::game_state_machine* state_machine,
					  mce::core::game_state* parent_state);
	/// Destroys the entity_game_state.
	virtual ~entity_game_state() = 0;

	/// Allows access to the contained entity_manager.
	const entity::entity_manager& entity_manager() const {
		return entity_manager_;
	}

	/// Allows access to the contained entity_manager.
	entity::entity_manager& entity_manager() {
		return entity_manager_;
	}
};

} /* namespace core */
} /* namespace mce */

#endif /* MCE_CORE_ENTITY_GAME_STATE_HPP_ */
