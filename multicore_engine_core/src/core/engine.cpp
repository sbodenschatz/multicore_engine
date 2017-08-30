/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/core/engine.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <chrono>
#include <fstream>
#include <mce/asset/asset_manager.hpp>
#include <mce/config/config_store.hpp>
#include <mce/core/core_defs.hpp>
#include <mce/core/engine.hpp>
#include <mce/core/game_state_machine.hpp>
#include <mce/core/system.hpp>
#include <mce/model/model_data_manager.hpp>
#include <sstream>

namespace mce {
namespace core {

engine::engine()
		: running_{false}, asset_manager_{std::make_unique<asset::asset_manager>()},
		  model_data_manager_{std::make_unique<model::model_data_manager>(asset_manager())} {
	initialize_config();
	game_state_machine_ = std::make_unique<mce::core::game_state_machine>(this);
}

engine::~engine() {
	game_state_machine_.reset(); // Ensure that the game_state_machine is cleaned up first.
}

void engine::initialize_config() {
	std::string user_cfg_name = "config.cfg";
	std::ifstream user_cfg(user_cfg_name);
	std::stringstream default_cfg("");
	config_store_ = std::make_unique<config::config_store>(
			user_cfg, default_cfg, [user_cfg_name](config::config_store::config_storer& cs) {
				std::ofstream user_cfg(user_cfg_name);
				cs.store(user_cfg);
			});
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
					 [](const auto& a, const auto& b) { return a.first > b.first; });
}

} // namespace core
} // namespace mce
