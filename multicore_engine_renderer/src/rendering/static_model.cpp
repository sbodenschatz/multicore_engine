/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/static_model.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/graphics/transfer_manager.hpp>
#include <mce/model/polygon_model.hpp>
#include <mce/rendering/model_manager.hpp>
#include <mce/rendering/static_model.hpp>

namespace mce {
namespace rendering {

static_model::~static_model() {}

void static_model::raise_error_flag(std::exception_ptr e) noexcept {
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

void static_model::complete_loading(const model::polygon_model_ptr& polygon_mdl) noexcept {
	std::unique_lock<std::mutex> lock(modification_mutex);
	meta_data_ = polygon_mdl->meta_data(); // TODO Convert to friendly format.
	vertex_index_buffer_ = graphics::buffer(
			mgr_.dev_, mgr_.mem_mgr_, mgr_.destruction_manager_, polygon_mdl->content_data_size(),
			vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer |
					vk::BufferUsageFlagBits::eTransferDst);
	auto this_shared = this->shared_from_this();
	current_state_ = state::staging;
	lock.unlock();
	mgr_.transfer_mgr_.upload_buffer(polygon_mdl->content_data_shared(), polygon_mdl->content_data_size(),
									 vertex_index_buffer_.native_buffer(), 0u,
									 [this_shared](vk::Buffer) { this_shared->complete_staging(); });
}

void static_model::complete_staging() noexcept {
	std::unique_lock<std::mutex> lock(modification_mutex);
	current_state_ = state::ready;
	auto this_shared = std::static_pointer_cast<const static_model>(this->shared_from_this());
	lock.unlock();
	// From here on the polygon model object is immutable and can therefore be read without holding a lock
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

} /* namespace rendering */
} /* namespace mce */
