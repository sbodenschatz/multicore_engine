/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/core/entity_game_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_CORE_ENTITY_GAME_STATE_HPP_
#define MCE_CORE_ENTITY_GAME_STATE_HPP_

#include <mce/core/game_state.hpp>
#include <mce/entity/entity_manager.hpp>

namespace mce {
namespace core {

class entity_game_state : public game_state {
	entity::entity_manager entity_manager_;

protected:
	template <typename T, typename... Args>
	T* add_system_state(Args&&... args) {
		auto s = game_state::add_system_state<T>(std::forward<Args>(args)...);
		s->register_to_entity_manager(entity_manager_);
		return s;
	}

public:
	entity_game_state(mce::core::engine* engine, mce::core::game_state_machine* state_machine,
					  mce::core::game_state* parent_state);
	virtual ~entity_game_state();

	const entity::entity_manager& entity_manager() const {
		return entity_manager_;
	}

	entity::entity_manager& entity_manager() {
		return entity_manager_;
	}
};

} /* namespace core */
} /* namespace mce */

#endif /* MCE_CORE_ENTITY_GAME_STATE_HPP_ */
