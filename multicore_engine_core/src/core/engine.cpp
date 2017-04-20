/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/engine.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/asset_manager.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <chrono>
#include <core/core_defs.hpp>
#include <core/engine.hpp>
#include <core/game_state_machine.hpp>
#include <core/system.hpp>

namespace mce {
namespace core {

engine::engine() : running_{true}, asset_manager_{std::make_unique<asset::asset_manager>()} {
	game_state_machine_ = std::make_unique<mce::core::game_state_machine>(this);
}

engine::~engine() {}

void engine::run() {
	auto old_t = std::chrono::high_resolution_clock::now();
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
	for(auto& sys : systems_) {
		sys.second->preprocess(frame_time);
	}
	game_state_machine_->process(frame_time);
	for(auto& sys : boost::adaptors::reverse(systems_)) {
		sys.second->postprocess(frame_time);
	}
}
void engine::render(const mce::core::frame_time& frame_time) {
	for(auto& sys : systems_) {
		sys.second->prerender(frame_time);
	}
	game_state_machine_->render(frame_time);
	for(auto& sys : boost::adaptors::reverse(systems_)) {
		sys.second->postrender(frame_time);
	}
}

} // namespace core
} // namespace mce
