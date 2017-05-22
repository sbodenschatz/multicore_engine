/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/engine.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <mce/asset/asset_manager.hpp>
#include <chrono>
#include <mce/core/core_defs.hpp>
#include <mce/core/engine.hpp>
#include <mce/core/game_state_machine.hpp>
#include <mce/core/system.hpp>

namespace mce {
namespace core {

engine::engine() : running_{false}, asset_manager_{std::make_unique<asset::asset_manager>()} {
	game_state_machine_ = std::make_unique<mce::core::game_state_machine>(this);
}

engine::~engine() {
	game_state_machine_.reset(); // Ensure that the game_state_machine is cleaned up first.
}

void engine::run() {
	auto old_t = std::chrono::high_resolution_clock::now();
	running_ = true;
	while(running()) {
		auto new_t = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> delta_t = new_t - old_t;
		old_t = new_t;
		frame_time ft{delta_t.count()};
		process(ft);
		render(ft);
	}
}
void engine::process(const mce::core::frame_time& frame_time) {
	for(auto& sys : systems_pre_phase_ordered) {
		sys.second->preprocess(frame_time);
	}
	game_state_machine_->process(frame_time);
	for(auto& sys : systems_post_phase_ordered) {
		sys.second->postprocess(frame_time);
	}
}
void engine::render(const mce::core::frame_time& frame_time) {
	for(auto& sys : systems_pre_phase_ordered) {
		sys.second->prerender(frame_time);
	}
	game_state_machine_->render(frame_time);
	for(auto& sys : systems_post_phase_ordered) {
		sys.second->postrender(frame_time);
	}
}

void engine::refresh_system_ordering() {
	std::stable_sort(systems_pre_phase_ordered.begin(), systems_pre_phase_ordered.end(),
					 [](const auto& a, const auto& b) { return a.first < b.first; });
	std::stable_sort(systems_post_phase_ordered.begin(), systems_post_phase_ordered.end(),
					 [](const auto& a, const auto& b) { return a.first < b.first; });
}

} // namespace core
} // namespace mce
