/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/game_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/core/game_state.hpp>
#include <mce/core/system_state.hpp>

namespace mce {
namespace core {

game_state::game_state(mce::core::engine* engine, mce::core::game_state_machine* state_machine,
					   mce::core::game_state* parent_state)
		: engine_{engine}, state_machine_{state_machine}, parent_state_{parent_state} {}

game_state::~game_state() {
	while(!system_states_.empty()) {
		system_states_.pop_back();
	}
}

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
void game_state::reenter(const boost::any&) {}

void game_state::process_leave_pop() {
	leave_pop();
	for(auto& sys_state : system_states_) {
		sys_state.second->leave_pop();
	}
}
void game_state::process_leave_push() {
	leave_push();
	for(auto& sys_state : system_states_) {
		sys_state.second->leave_push();
	}
}
void game_state::process_reenter(const boost::any& parameter) {
	process_reenter(parameter);
	for(auto& sys_state : system_states_) {
		sys_state.second->reenter(parameter);
	}
}

} /* namespace core */
} /* namespace mce */
