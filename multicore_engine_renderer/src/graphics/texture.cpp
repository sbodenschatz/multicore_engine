/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/graphics/texture.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4310)
#pragma warning(disable : 4458)
#pragma warning(disable : 4100)
#endif
#include <gli/gli.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <mce/graphics/texture.hpp>

namespace mce {
namespace graphics {

texture::~texture() {}

void texture::raise_error_flag(std::exception_ptr e) noexcept {
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

} /* namespace graphics */
} /* namespace mce */
