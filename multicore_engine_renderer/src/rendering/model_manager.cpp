/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/model_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/model/model_data_manager.hpp>
#include <mce/rendering/model_manager.hpp>
#include <mce/rendering/static_model.hpp>
#include <thread>

namespace mce {
namespace rendering {

model_manager::~model_manager() {
	std::weak_ptr<const detail::model_manager_dependencies> md = dependencies_;
	dependencies_.reset();
	while(!md.expired()) {
		// A callback has locked the dependencies object. If we just would proceed, it would keep the
		// dependencies object alive and it might outlive the objects it references. Unfortunately we can only
		// wait for completion by spinning because we have no place to put a condition variable here.
		std::this_thread::yield();
	}
}

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
			auto tmp = std::make_shared<static_model>(dependencies_, name);
			loaded_static_models_[name] = tmp;
			lock.unlock();
			dependencies_->model_data_mgr_.load_polygon_model(
					name,
					[tmp](const model::polygon_model_ptr& poly_model) { tmp->complete_loading(poly_model); },
					[tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			return tmp;
		}
	}
}

} /* namespace rendering */
} /* namespace mce */
