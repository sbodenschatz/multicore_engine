/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/game_state_machine.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <core/game_state_machine.hpp>
#include <util/unused.hpp>

namespace mce {
namespace core {

game_state_machine::game_state_machine(mce::core::engine* engine) : engine{engine} {}

game_state_machine::~game_state_machine() {}

void game_state_machine::process(const mce::core::frame_time& frame_time) {
	// TODO: Implement
	UNUSED(frame_time);
}
void game_state_machine::render(const mce::core::frame_time& frame_time) {
	// TODO: Implement
	UNUSED(frame_time);
}

} /* namespace core */
} /* namespace mce */
