/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/material.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/exceptions.hpp>
#include <mce/graphics/descriptor_set.hpp>
#include <mce/graphics/texture_manager.hpp>
#include <mce/rendering/material.hpp>
#include <mce/rendering/material_manager.hpp>

namespace mce {
namespace rendering {

material::~material() {}

void material::raise_error_flag(std::exception_ptr e) noexcept {
	current_state_ = state::error;
	// Sync with other threads working on the lock to ensure all other threads have seen the update to
	// current_state_ after that drop lock again to not hold it during callback execution. After that the
	// handlers containers can be modified by this thread because no other threads are using them anymore.
	{ std::unique_lock<std::mutex> lock(modification_mutex); }
	for(auto& handler : error_handlers) {
		try {
			handler(e);
		} catch(...) {
			// Drop exceptions escaped from completion handlers
		}
	}
	error_handlers.clear();
	completion_handlers.clear();
	error_handlers.shrink_to_fit();
	completion_handlers.shrink_to_fit();
}

void material::try_raise_error_flag(std::exception_ptr e) noexcept {
	state expected = current_state_;
	if(expected == state::ready || expected == state::error) return;
	if(!current_state_.compare_exchange_strong(expected, state::error)) return;
	// Sync with other threads working on the lock to ensure all other threads have seen the update to
	// current_state_ after that drop lock again to not hold it during callback execution. After that the
	// handlers containers can be modified by this thread because no other threads are using them anymore.
	{ std::unique_lock<std::mutex> lock(modification_mutex); }
	for(auto& handler : error_handlers) {
		try {
			handler(e);
		} catch(...) {
			// Drop exceptions escaped from completion handlers
		}
	}
	error_handlers.clear();
	completion_handlers.clear();
	error_handlers.shrink_to_fit();
	completion_handlers.shrink_to_fit();
}

bool material::try_start_loading(graphics::texture_manager& mgr,
								 const material_description& description) noexcept {
	state expected = state::initial;
	if(current_state_.compare_exchange_strong(expected, state::loading)) {
		try {
			auto tmp = shared_from_this();
			albedo_map_ =
					mgr.load_texture(description.albedo_map_name,
									 [tmp](const graphics::texture_ptr& tex) { tmp->texture_loaded(tex); },
									 [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			normal_map_ =
					mgr.load_texture(description.normal_map_name,
									 [tmp](const graphics::texture_ptr& tex) { tmp->texture_loaded(tex); },
									 [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			material_map_ =
					mgr.load_texture(description.material_map_name,
									 [tmp](const graphics::texture_ptr& tex) { tmp->texture_loaded(tex); },
									 [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			emission_map_ =
					mgr.load_texture(description.emission_map_name,
									 [tmp](const graphics::texture_ptr& tex) { tmp->texture_loaded(tex); },
									 [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
			return true;
		} catch(...) {
			raise_error_flag(std::current_exception());
			return false;
		}
	} else {
		return false;
	}
}

void material::texture_loaded(const graphics::texture_ptr&) noexcept {
	std::unique_lock<std::mutex> lock(modification_mutex);
	if(current_state_ == state::loading && albedo_map_->ready() && normal_map_->ready() &&
	   material_map_->ready() && emission_map_->ready()) {
		auto this_shared = std::static_pointer_cast<const material>(this->shared_from_this());
		current_state_ = state::ready;
		lock.unlock();
		// From here on the material object is immutable and can therefore be read without holding a lock
		for(auto& handler : completion_handlers) {
			try {
				handler(this_shared);
			} catch(...) {
				// Drop exceptions escaped from completion handlers
			}
		}
		completion_handlers.clear();
		error_handlers.clear();
		completion_handlers.shrink_to_fit();
		error_handlers.shrink_to_fit();
	}
}

void material::process_pending_material_loads(graphics::texture_manager& tex_mgr,
											  const material_library_ptr& lib) {
	std::unique_lock<std::mutex> lock(modification_mutex);
	if(current_state_ == material::state::initial) {
		auto desc = lib->find_material_description(name_);
		if(desc) {
			lock.unlock();
			try_start_loading(tex_mgr, *desc);
			--pending_lib_load_count;
			return;
		}
	}
	if(--pending_lib_load_count == 0) check_load_fails();
}

void material::check_load_fails() {
	std::unique_lock<std::mutex> lock(modification_mutex);
	if(pending_lib_load_count > 0) return;
	if(current_state_ == material::state::initial) {
		lock.unlock();
		try_raise_error_flag(std::make_exception_ptr(
				mce::path_not_found_exception("Material '" + name_ + "' not found.")));
	}
}
void material::bind(graphics::descriptor_set& ds) const {
	ds.update()(0, 0, vk::DescriptorType::eCombinedImageSampler,
				{albedo_map_->bind(), normal_map_->bind(), material_map_->bind(), emission_map_->bind()});
}

} /* namespace rendering */
} /* namespace mce */
