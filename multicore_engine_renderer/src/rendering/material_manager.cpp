/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/material_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/asset/asset_manager.hpp>
#include <mce/rendering/material_manager.hpp>

namespace mce {
namespace rendering {

material_manager::~material_manager() {}

void material_manager::process_pending_material_loads(const material_library_ptr& lib) {
	std::shared_lock<std::shared_timed_mutex> lock(rw_lock_);
	// Collect pending loads first and attempt to run them after dropping the lock to avoid holding a lock
	// while calling callbacks for recursion and deadlock avoidance.
	std::vector<std::pair<std::shared_ptr<material>, const material_description*>> pending_loads;
	for(const auto& mat : loaded_materials_) {
		if(mat.second->current_state() == material::state::initial) {
			auto desc = lib->find_material_description(mat.second->name());
			if(desc) {
				pending_loads.emplace_back(mat.second, desc);
			}
		}
	}
	lock.unlock();
	for(const auto& load : pending_loads) {
		load.first->try_start_loading(tex_mgr, *(load.second));
	}
}

std::shared_ptr<material_library> material_manager::internal_load_material_lib(const std::string& name) {
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(rw_lock_);
		auto it = loaded_material_libs_.find(name);
		if(it != loaded_material_libs_.end()) {
			return it->second;
		}
	}
	{
		// Acquire write lock
		std::unique_lock<std::shared_timed_mutex> lock(rw_lock_);
		// Double check if the material lib is still not in the map.
		auto it = loaded_material_libs_.find(name);
		if(it != loaded_material_libs_.end()) {
			return it->second;
		} else {
			auto tmp = std::make_shared<material_library>(name);
			loaded_material_libs_[name] = tmp;
			lock.unlock();
			amgr.load_asset_async(
					name, [tmp](const asset::asset_ptr& lib_asset) { tmp->complete_loading(lib_asset); },
					[tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			return tmp;
		}
	}
}

} /* namespace rendering */
} /* namespace mce */
