/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/model/model_manager.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <asset/asset_manager.hpp>
#include <model/model_manager.hpp>
#include <util/unused.hpp>

namespace mce {
namespace model {

polygon_model_ptr model_manager::load_polygon_model(const std::string& name) {
	return internal_load_polygon_model(name);
}
collision_model_ptr model_manager::load_collision_model(const std::string& name) {
	return internal_load_collision_model(name);
}
std::shared_ptr<polygon_model> model_manager::internal_load_polygon_model(const std::string& name) {
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(loaded_polygon_models_rw_lock);
		auto it = loaded_polygon_models.find(name);
		if(it != loaded_polygon_models.end()) {
			return it->second;
		}
	}
	{
		// Acquire write lock
		std::unique_lock<std::shared_timed_mutex> lock(loaded_polygon_models_rw_lock);
		// Double check if the polygon model is still not in the map.
		auto it = loaded_polygon_models.find(name);
		if(it != loaded_polygon_models.end()) {
			return it->second;
		} else {
			auto tmp = std::make_shared<polygon_model>(name);
			loaded_polygon_models[name] = tmp;
			asset_manager.load_asset_async(name + ".model",
										   [tmp, this](const asset::asset_ptr& polygon_asset) {
											   tmp->complete_loading(polygon_asset, *this);
										   });
			return tmp;
		}
	}
}
std::shared_ptr<collision_model> model_manager::internal_load_collision_model(const std::string& name) {
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(loaded_collision_models_rw_lock);
		auto it = loaded_collision_models.find(name);
		if(it != loaded_collision_models.end()) {
			return it->second;
		}
	}
	{
		// Acquire write lock
		std::unique_lock<std::shared_timed_mutex> lock(loaded_collision_models_rw_lock);
		// Double check if the collision model is still not in the map.
		auto it = loaded_collision_models.find(name);
		if(it != loaded_collision_models.end()) {
			return it->second;
		} else {
			auto tmp = std::make_shared<collision_model>(name);
			loaded_collision_models[name] = tmp;
			asset_manager.load_asset_async(name + ".col", [tmp](const asset::asset_ptr& collision_asset) {
				tmp->complete_loading(collision_asset);
			});
			return tmp;
		}
	}
}
void model_manager::start_stage_polygon_model(std::shared_ptr<polygon_model> model) {
	// TODO: Initiate upload to GPU memory through renderer and do the call below when upload is finished.
	model->complete_staging(*this);
}

} // namespace model
} // namespace mce
