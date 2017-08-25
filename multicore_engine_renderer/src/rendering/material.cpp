/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/material.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/texture_manager.hpp>
#include <mce/rendering/material.hpp>
#include <mce/rendering/material_manager.hpp>

namespace mce {
namespace rendering {

material::~material() {}

void material::raise_error_flag(std::exception_ptr e) noexcept {
	current_state_ = state::error;
	std::unique_lock<std::mutex> lock(modification_mutex);
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

material::material(material_manager& mgr, const material_description& description)
		: current_state_{state::loading}, name_{description.name} {
	auto tmp = shared_from_this();
	albedo_map_ =
			mgr.tex_mgr.load_texture(description.albedo_map_name,
									 [tmp](const graphics::texture_ptr& tex) { tmp->texture_loaded(tex); },
									 [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
	normal_map_ =
			mgr.tex_mgr.load_texture(description.normal_map_name,
									 [tmp](const graphics::texture_ptr& tex) { tmp->texture_loaded(tex); },
									 [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
	material_map_ =
			mgr.tex_mgr.load_texture(description.material_map_name,
									 [tmp](const graphics::texture_ptr& tex) { tmp->texture_loaded(tex); },
									 [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
	emission_map_ =
			mgr.tex_mgr.load_texture(description.emission_map_name,
									 [tmp](const graphics::texture_ptr& tex) { tmp->texture_loaded(tex); },
									 [tmp](std::exception_ptr e) { tmp->raise_error_flag(e); });
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

} /* namespace rendering */
} /* namespace mce */
