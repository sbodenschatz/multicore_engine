/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/model/model_loading.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <asset/asset_manager.hpp>
#include <asset/load_unit_asset_loader.hpp>
#include <asset/pack_file_reader.hpp>
#include <atomic>
#include <boost/test/unit_test.hpp>
#include <model/model_manager.hpp>
#include <thread>
#include <vector>

namespace mce {
namespace model {

BOOST_AUTO_TEST_SUITE(model)
BOOST_AUTO_TEST_SUITE(collision_model_test)

BOOST_AUTO_TEST_CASE(load_example_collision_model) {
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
	BOOST_CHECK(load_unit_ready);
	BOOST_CHECK(!load_unit_failed);
	if(load_unit_failed) return;
	model_manager mm(am);
	std::atomic<bool> model_loaded{false};
	std::atomic<bool> model_failed{false};
	mm.load_collision_model("models/cube",
							[&model_loaded](const collision_model_ptr&) { model_loaded = true; },
							[&model_failed](std::exception_ptr) { model_failed = true; });
	while(!model_loaded && !model_failed) {
		std::this_thread::sleep_for(1ms);
	}
	BOOST_CHECK(model_loaded);
	BOOST_CHECK(!model_failed);
}

BOOST_AUTO_TEST_CASE(load_example_polygon_model) {
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
	BOOST_CHECK(load_unit_ready);
	BOOST_CHECK(!load_unit_failed);
	model_manager mm(am);
	std::atomic<bool> model_loaded{false};
	std::atomic<bool> model_failed{false};
	mm.load_polygon_model("models/cube", [&model_loaded](const polygon_model_ptr&) { model_loaded = true; },
						  [&model_failed](std::exception_ptr) { model_failed = true; });
	while(!model_loaded && !model_failed) {
		std::this_thread::sleep_for(1ms);
	}
	BOOST_CHECK(model_loaded);
	BOOST_CHECK(!model_failed);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace model
} // namespace mce
