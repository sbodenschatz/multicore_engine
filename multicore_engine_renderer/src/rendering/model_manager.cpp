/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/model_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/model/model_data_manager.hpp>
#include <mce/rendering/model_manager.hpp>
#include <mce/rendering/static_model.hpp>

namespace mce {
namespace rendering {

model_manager::~model_manager() {}

std::shared_ptr<static_model> model_manager::internal_load_static_model(const std::string& name) {
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(loaded_static_models_rw_lock_);
		auto it = loaded_static_models_.find(name);
		if(it != loaded_static_models_.end()) {
			return it->second;
		}
	}
	{
		// Acquire write lock
		std::unique_lock<std::shared_timed_mutex> lock(loaded_static_models_rw_lock_);
		// Double check if the static model is still not in the map.
		auto it = loaded_static_models_.find(name);
		if(it != loaded_static_models_.end()) {
			return it->second;
		} else {
			auto tmp = std::make_shared<static_model>(*this, name);
			loaded_static_models_[name] = tmp;
			model_data_mgr_.load_polygon_model(name,
											   [tmp](const model::polygon_model_ptr& collision_asset) {
												   tmp->complete_loading(collision_asset);
											   },
											   [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			return tmp;
		}
	}
}

} /* namespace rendering */
} /* namespace mce */
