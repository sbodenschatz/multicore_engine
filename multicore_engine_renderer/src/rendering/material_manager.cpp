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
			dependencies_->amgr.load_asset_async(
					name, [tmp](const asset::asset_ptr& lib_asset) { tmp->complete_loading(lib_asset); },
					[tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			return tmp;
		}
	}
}
std::shared_ptr<material> material_manager::internal_load_material(const std::string& name) {
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(rw_lock_);
		auto it = loaded_materials_.find(name);
		if(it != loaded_materials_.end()) {
			return it->second;
		}
	}
	{
		// Acquire write lock
		std::unique_lock<std::shared_timed_mutex> lock(rw_lock_);
		// Double check if the material is still not in the map.
		auto it = loaded_materials_.find(name);
		if(it != loaded_materials_.end()) {
			return it->second;
		} else {
			const material_description* desc = nullptr;
			std::vector<std::shared_ptr<material_library>> pending_libs;
			for(const auto& lib : loaded_material_libs_) {
				if(lib.second->ready()) {
					desc = lib.second->find_material_description(name);
				} else {
					pending_libs.push_back(lib.second);
				}
			}
			if(!desc && pending_libs.empty()) {
				throw mce::path_not_found_exception("Material '" + name + "' not found.");
			}
			auto tmp = std::make_shared<material>(name);
			loaded_materials_[name] = tmp;
			if(desc) {
				tmp->try_start_loading(dependencies_->tex_mgr, *desc);
				return tmp;
			}
			lock.unlock();
			tmp->pending_lib_load_count += pending_libs.size();
			std::weak_ptr<const detail::material_manager_dependencies> weak_deps = dependencies_;
			for(const auto& pl : pending_libs) {
				pl->run_when_ready(
						[tmp, weak_deps](const material_library_ptr& lib) {
							auto md = weak_deps.lock();
							if(md) {
								tmp->process_pending_material_loads(md->tex_mgr, lib);
							} else {
								tmp->raise_error_flag(std::make_exception_ptr(mce::async_state_exception(
										"Manager object expired when required by callback.")));
							}
						},
						[tmp](std::exception_ptr) {
							if(--(tmp->pending_lib_load_count) == 0) tmp->check_load_fails();
						});
			}
			return tmp;
		}
	}
}

} /* namespace rendering */
} /* namespace mce */
