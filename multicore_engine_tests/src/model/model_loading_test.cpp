/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/model/model_loading.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <atomic>
#include <gtest.hpp>
#include <mce/asset/asset_manager.hpp>
#include <mce/asset/load_unit_asset_loader.hpp>
#include <mce/asset/pack_file_reader.hpp>
#include <mce/model/model_data_manager.hpp>
#include <thread>
#include <vector>

namespace mce {
namespace model {

TEST(model_collision_model_test, load_example_collision_model) {
	using namespace std::chrono_literals;
	asset::asset_manager am;
	auto loader = std::make_shared<asset::load_unit_asset_loader>(std::vector<asset::path_prefix>(
			{{std::make_unique<asset::pack_file_reader>(), "../multicore_engine_assets/assets.pack"}}));
	am.add_asset_loader(loader);
	std::atomic<bool> load_unit_ready{false};
	std::atomic<bool> load_unit_failed{false};
	am.start_pin_load_unit("models_col", [&load_unit_ready]() { load_unit_ready = true; },
						   [&load_unit_failed](std::exception_ptr) { load_unit_failed = true; });
	while(!load_unit_ready && !load_unit_failed) {
		std::this_thread::sleep_for(1ms);
	}
	ASSERT_TRUE(load_unit_ready);
	ASSERT_TRUE(!load_unit_failed);
	if(load_unit_failed) return;
	model_data_manager mm(am);
	std::atomic<bool> model_loaded{false};
	std::atomic<bool> model_failed{false};
	mm.load_collision_model("models/cube",
							[&model_loaded](const collision_model_ptr&) { model_loaded = true; },
							[&model_failed](std::exception_ptr) { model_failed = true; });
	while(!model_loaded && !model_failed) {
		std::this_thread::sleep_for(1ms);
	}
	ASSERT_TRUE(model_loaded);
	ASSERT_TRUE(!model_failed);
}

TEST(model_collision_model_test, load_example_polygon_model) {
	using namespace std::chrono_literals;
	asset::asset_manager am;
	auto loader = std::make_shared<asset::load_unit_asset_loader>(std::vector<asset::path_prefix>(
			{{std::make_unique<asset::pack_file_reader>(), "../multicore_engine_assets/assets.pack"}}));
	am.add_asset_loader(loader);
	std::atomic<bool> load_unit_ready{false};
	std::atomic<bool> load_unit_failed{false};
	am.start_pin_load_unit("models_geo", [&load_unit_ready]() { load_unit_ready = true; },
						   [&load_unit_failed](std::exception_ptr) { load_unit_failed = true; });
	while(!load_unit_ready && !load_unit_failed) {
		std::this_thread::sleep_for(1ms);
	}
	ASSERT_TRUE(load_unit_ready);
	ASSERT_TRUE(!load_unit_failed);
	model_data_manager mm(am);
	std::atomic<bool> model_loaded{false};
	std::atomic<bool> model_failed{false};
	mm.load_polygon_model("models/cube", [&model_loaded](const polygon_model_ptr&) { model_loaded = true; },
						  [&model_failed](std::exception_ptr) { model_failed = true; });
	while(!model_loaded && !model_failed) {
		std::this_thread::sleep_for(1ms);
	}
	ASSERT_TRUE(model_loaded);
	ASSERT_TRUE(!model_failed);
}

} // namespace model
} // namespace mce
