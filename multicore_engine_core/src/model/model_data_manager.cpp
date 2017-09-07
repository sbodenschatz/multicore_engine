/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/model/model_manager.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <boost/container/vector.hpp>
#include <exception>
#include <mce/asset/asset.hpp>
#include <mce/asset/asset_defs.hpp>
#include <mce/asset/asset_manager.hpp>
#include <mce/model/model_data_manager.hpp>
#include <memory>
#include <mutex>
#include <string>

namespace mce {
namespace model {

polygon_model_ptr model_data_manager::load_polygon_model(const std::string& name) {
	return internal_load_polygon_model(name);
}
collision_model_ptr model_data_manager::load_collision_model(const std::string& name) {
	return internal_load_collision_model(name);
}
std::shared_ptr<polygon_model> model_data_manager::internal_load_polygon_model(const std::string& name) {
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
			lock.unlock();
			asset_manager.load_asset_async(
					name + ".model",
					[tmp](const asset::asset_ptr& polygon_asset) { tmp->complete_loading(polygon_asset); },
					[tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			return tmp;
		}
	}
}
std::shared_ptr<collision_model> model_data_manager::internal_load_collision_model(const std::string& name) {
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
			lock.unlock();
			asset_manager.load_asset_async(name + ".col",
										   [tmp](const asset::asset_ptr& collision_asset) {
											   tmp->complete_loading(collision_asset);
										   },
										   [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			return tmp;
		}
	}
}

} // namespace model
} // namespace mce
