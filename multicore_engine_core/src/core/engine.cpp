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
#include <mce/util/statistics.hpp>
#include <sstream>

namespace mce {
namespace core {

namespace detail {

struct engine_core_stats_pimpl {
	std::shared_ptr<config::variable<int>> enable_frame_time_stat;
	std::shared_ptr<util::aggregate_statistic<std::chrono::microseconds::rep>> frame_time_aggregate;
	std::shared_ptr<util::histogram_statistic<std::chrono::microseconds::rep>> frame_time_histogram;
};

} // namespace detail

engine::engine()
		: max_general_concurrency_{std::thread::hardware_concurrency()}, running_{false},
		  engine_metadata_{"mce", get_build_version_number()},
		  application_metadata_{"mce-app", get_build_version_number()},
		  statistics_manager_{std::make_unique<util::statistics_manager>()},
		  asset_manager_{std::make_unique<asset::asset_manager>()},
		  model_data_manager_{std::make_unique<model::model_data_manager>(asset_manager())} {
	initialize_config();
	stats_pimpl_ = std::make_unique<detail::engine_core_stats_pimpl>();
	stats_pimpl_->enable_frame_time_stat = config_store_->resolve("stats.core.frametime", 0);
	initialize_stats();
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

void engine::initialize_stats() {
	if(stats_pimpl_->enable_frame_time_stat->value()) {
		stats_pimpl_->frame_time_aggregate =
				statistics_manager_->create<util::aggregate_statistic<std::chrono::microseconds::rep>>(
						"core.frametime.aggregate");
		auto frametime_buckets = config_store_->resolve("stats.core.frametime.buckets", 1000);
		auto frametime_max = config_store_->resolve("stats.core.frametime.max", 20000);
		stats_pimpl_->frame_time_histogram =
				statistics_manager_->create<util::histogram_statistic<std::chrono::microseconds::rep>>(
						"core.frametime.histogram", 0, frametime_max->value(), frametime_buckets->value());
	}
}

void engine::run() {
	tbb_concurrency_control = std::make_unique<oneapi::tbb::global_control>(
			oneapi::tbb::global_control::max_allowed_parallelism, max_general_concurrency_);
	core::clock clk;
	running_ = true;
	while(running()) {
		auto ft = clk.frame_tick();
		if(stats_pimpl_->enable_frame_time_stat->value()) {
			stats_pimpl_->frame_time_aggregate->record(ft.delta_t_microseconds.count());
			stats_pimpl_->frame_time_histogram->record(ft.delta_t_microseconds.count());
		}
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
