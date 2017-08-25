/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/material_library.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/rendering/material_library.hpp>

namespace mce {
namespace rendering {

material_library::~material_library() {}

void material_library::raise_error_flag(std::exception_ptr e) noexcept {
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

} /* namespace rendering */
} /* namespace mce */
