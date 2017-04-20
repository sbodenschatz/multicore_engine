/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/game_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <core/game_state.hpp>
#include <core/system_state.hpp>

namespace mce {
namespace core {

game_state::~game_state() {}

void game_state::process(const mce::core::frame_time& frame_time) {
	preprocess(frame_time);
	for(auto& sys_state : system_states_) {
		sys_state.second->process(frame_time);
	}
	postprocess(frame_time);
}

void game_state::render(const mce::core::frame_time& frame_time) {
	prerender(frame_time);
	for(auto& sys_state : system_states_) {
		sys_state.second->render(frame_time);
	}
	postrender(frame_time);
}

void game_state::preprocess(const mce::core::frame_time&) {}
void game_state::postprocess(const mce::core::frame_time&) {}
void game_state::prerender(const mce::core::frame_time&) {}
void game_state::postrender(const mce::core::frame_time&) {}

void game_state::leave_pop() {}
void game_state::leave_push() {}
void game_state::reenter() {}

} /* namespace core */
} /* namespace mce */
