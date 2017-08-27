/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/texture_manager.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/asset/asset_manager.hpp>
#include <mce/graphics/texture_manager.hpp>
#include <thread>

namespace mce {
namespace graphics {

texture_manager::~texture_manager() {
	std::weak_ptr<const detail::texture_manager_dependencies> md = dependencies_;
	dependencies_.reset();
	while(!md.expired()) {
		// A callback has locked the dependencies object. If we just would proceed, it would keep the
		// dependencies object alive and it might outlive the objects it references. Unfortunately we can only
		// wait for completion by spinning because we have no place to put a condition variable here.
		std::this_thread::yield();
	}
}

std::shared_ptr<texture> texture_manager::internal_load_texture(const std::string& name) {
	{
		// Acquire read lock
		std::shared_lock<std::shared_timed_mutex> lock(loaded_textures_rw_lock_);
		auto it = loaded_textures_.find(name);
		if(it != loaded_textures_.end()) {
			return it->second;
		}
	}
	{
		// Acquire write lock
		std::unique_lock<std::shared_timed_mutex> lock(loaded_textures_rw_lock_);
		// Double check if the texture is still not in the map.
		auto it = loaded_textures_.find(name);
		if(it != loaded_textures_.end()) {
			return it->second;
		} else {
			auto tmp = std::make_shared<texture>(dependencies_, name);
			loaded_textures_[name] = tmp;
			lock.unlock();
			dependencies_->asset_mgr_.load_asset_async(
					name,
					[tmp](const asset::asset_ptr& texture_asset) { tmp->complete_loading(texture_asset); },
					[tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			return tmp;
		}
	}
}

} /* namespace graphics */
} /* namespace mce */
