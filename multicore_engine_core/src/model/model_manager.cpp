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
	// TODO: Implement
	UNUSED(name);
	return nullptr;
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
			asset_manager.load_asset_async(name, [tmp](const asset::asset_ptr& collision_asset) {
				tmp->complete_loading(collision_asset);
			});
			return tmp;
		}
	}
}

} // namespace model
} // namespace mce
