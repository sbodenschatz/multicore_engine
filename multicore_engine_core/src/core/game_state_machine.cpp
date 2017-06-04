/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/game_state_machine.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/game_state.hpp>
#include <mce/core/game_state_machine.hpp>

namespace mce {
namespace core {

game_state_machine::game_state_machine(mce::core::engine* engine)
		: engine{engine}, state_machine({engine, this}) {}

game_state_machine::~game_state_machine() {}

void game_state_machine::process(const mce::core::frame_time& frame_time) {
	auto s = state_machine.current_state();
	if(s) s->process(frame_time);
}
void game_state_machine::render(const mce::core::frame_time& frame_time) {
	auto s = state_machine.current_state();
	if(s) s->render(frame_time);
}

} /* namespace core */
} /* namespace mce */
