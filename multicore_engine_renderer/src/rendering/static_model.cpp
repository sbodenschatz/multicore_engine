/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/static_model.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <iterator>
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
	try {
		poly_model_ = polygon_mdl;
		meshes_.reserve(polygon_mdl->meta_data().meshes.size());
		std::transform(polygon_mdl->meta_data().meshes.begin(), polygon_mdl->meta_data().meshes.end(),
					   std::back_inserter(meshes_), [this](const model::static_model_mesh_meta_data& data) {
						   if(data.index_data_in_content.length() % 4)
							   throw mce::invalid_data_exception("The index data section for " +
																 data.group_name + "." + data.object_name +
																 " has a size not divisible by 4");
						   auto vertex_count = data.index_data_in_content.length() / 4;
						   if(vertex_count > std::numeric_limits<uint32_t>::max())
							   throw mce::invalid_data_exception("The size of the index data section for " +
																 data.group_name + "." + data.object_name +
																 " is too big (must fit in uint32_t).");
						   return mesh(this, data.object_name, data.group_name, data.material_name,
									   data.index_data_in_content.begin(), uint32_t(vertex_count));
					   });
		vertex_index_buffer_ = graphics::buffer(
				mgr_.dev_, mgr_.mem_mgr_, mgr_.destruction_manager_, polygon_mdl->content_data_size(),
				vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer |
						vk::BufferUsageFlagBits::eTransferDst);
	} catch(...) {
		raise_error_flag(std::current_exception());
	}

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
	// From here on the static model object is immutable and can therefore be read without holding a lock
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

void static_model::mesh::bind_vertices(vk::CommandBuffer cmd_buf) const {
	parent_->bind_vertices(cmd_buf);
}
void static_model::mesh::bind_indices(vk::CommandBuffer cmd_buf) const {
	cmd_buf.bindIndexBuffer(parent_->vertex_index_buffer_.native_buffer(), offset_, vk::IndexType::eUint32);
}
void static_model::mesh::record_draw_call(vk::CommandBuffer cmd_buf, uint32_t instances) const {
	cmd_buf.drawIndexed(vertex_count_, instances, 0, 0, 0);
}
void static_model::mesh::draw(vk::CommandBuffer cmd_buf, uint32_t instances) const {
	bind_vertices(cmd_buf);
	bind_indices(cmd_buf);
	record_draw_call(cmd_buf, instances);
}
void static_model::bind_vertices(vk::CommandBuffer cmd_buf) const {
	cmd_buf.bindVertexBuffers(0, vertex_index_buffer_.native_buffer(), vk::DeviceSize(0));
}
void static_model::bind_indices(vk::CommandBuffer cmd_buf, size_t mesh_index) const {
	meshes_.at(mesh_index).bind_indices(cmd_buf);
}
void static_model::record_draw_call(vk::CommandBuffer cmd_buf, size_t mesh_index, uint32_t instances) const {
	meshes_.at(mesh_index).record_draw_call(cmd_buf, instances);
}
void static_model::draw_model_mesh(vk::CommandBuffer cmd_buf, size_t mesh_index, uint32_t instances) const {
	meshes_.at(mesh_index).draw(cmd_buf, instances);
}

} /* namespace rendering */
} /* namespace mce */
