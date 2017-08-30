/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/entity_game_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/entity_game_state.hpp>

namespace mce {
namespace core {

entity_game_state::entity_game_state(mce::core::engine* engine, mce::core::game_state_machine* state_machine,
									 mce::core::game_state* parent_state)
		: game_state(engine, state_machine, parent_state), entity_manager_(engine) {}

entity_game_state::~entity_game_state() {}

} /* namespace core */
} /* namespace mce */
