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
#include <mce/core/version.hpp>
#include <mce/model/model_data_manager.hpp>
#include <sstream>
#include <tbb/task_scheduler_init.h>

namespace mce {
namespace core {

engine::engine()
		: max_general_concurrency_{std::thread::hardware_concurrency()}, running_{false},
		  engine_metadata_{"mce", get_build_version_number()},
		  application_metadata_{"mce-app", get_build_version_number()},
		  asset_manager_{std::make_unique<asset::asset_manager>()},
		  model_data_manager_{std::make_unique<model::model_data_manager>(asset_manager())} {
	initialize_config();
	game_state_machine_ = std::make_unique<mce::core::game_state_machine>(this);
}

engine::~engine() {
	game_state_machine_.reset(); // Ensure that the game_state_machine is cleaned up first.
	while(!systems_.empty()) {
		systems_.pop_back();
	}
}

void engine::initialize_config() {
	std::string user_cfg_name = "config.cfg";
	std::ifstream user_cfg(user_cfg_name);
	std::stringstream default_cfg("default.cfg");
	config_store_ = std::make_unique<config::config_store>(
			user_cfg, default_cfg, [user_cfg_name](config::config_store::config_storer& cs) {
				std::ofstream user_cfg(user_cfg_name);
				cs.store(user_cfg);
			});
}

void engine::run() {
	tsi = std::make_unique<tbb::task_scheduler_init>(max_general_concurrency_);
	core::clock clk;
	running_ = true;
	while(running()) {
		auto ft = clk.frame_tick();
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
