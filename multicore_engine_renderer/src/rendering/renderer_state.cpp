/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/renderer_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <cassert>
#include <glm/gtc/matrix_transform.hpp>
#include <mce/entity/entity_manager.hpp>
#include <mce/graphics/graphics_system.hpp>
#include <mce/graphics/pipeline.hpp>
#include <mce/graphics/pipeline_layout.hpp>
#include <mce/rendering/renderer_state.hpp>
#include <mce/util/algorithm.hpp>
#include <tbb/parallel_sort.h>

namespace mce {
namespace rendering {

renderer_state::renderer_state(core::system* sys) : system_state(sys) {}

renderer_state::~renderer_state() {}

void renderer_state::register_to_entity_manager(entity::entity_manager& em) {
	REGISTER_COMPONENT_TYPE_SIMPLE(em, camera, this->create_camera_component(owner, config), this);
	REGISTER_COMPONENT_TYPE_SIMPLE(em, point_light, this->create_point_light_component(owner, config), this);
	REGISTER_COMPONENT_TYPE_SIMPLE(em, static_model,
								   this->create_static_model_component(*this, owner, config), this);
}
void renderer_state::record_per_scene_data(renderer_system::per_frame_per_thread_data_t& local_data,
										   renderer_system::per_frame_data_t& frame_data) const {
	auto sys = static_cast<renderer_system*>(system_);
	sys->main_forward_pipeline_->bind(local_data.command_buffer.get());
	graphics::descriptor_set::bind(local_data.command_buffer.get(), *(sys->pipeline_layout_scene_pass_), 0,
								   {frame_data.scene_descriptor_set.get()}, {});
}
void renderer_state::record_per_material_data(
		const material* used_material, renderer_system::per_frame_per_thread_data_t& local_data) const {
	// TODO: Implement
	static_cast<void>(used_material);
	static_cast<void>(local_data);
}
void renderer_state::record_per_mesh_data(const static_model::mesh* used_mesh,
										  renderer_system::per_frame_per_thread_data_t& local_data) const {
	used_mesh->bind_vertices(local_data.command_buffer.get());
	used_mesh->bind_indices(local_data.command_buffer.get());
}
void renderer_state::record_render_task(const render_task& task,
										renderer_system::per_frame_per_thread_data_t& local_data) const {
	auto sys = static_cast<renderer_system*>(system_);
	local_data.command_buffer->pushConstants<decltype(task.push_constants)>(
			sys->pipeline_layout_scene_pass_->native_layout(), vk::ShaderStageFlagBits::eAllGraphics, 0,
			task.push_constants);
	task.used_mesh->record_draw_call(local_data.command_buffer.get());
}
void renderer_state::collect_scene_uniforms() {
	auto sys = static_cast<renderer_system*>(system_);
	// TODO: Implement camera selection.
	auto cam = camera_comps.begin();
	scene_uniforms.view = glm::inverse(cam->owner().calculate_transform());
	scene_uniforms.projection = glm::perspectiveFov(cam->fov(), float(sys->gs_.window().swapchain_size().x),
													float(sys->gs_.window().swapchain_size().y),
													cam->near_plane(), cam->far_plane());
	/// TODO: Implement light collection.
}
void renderer_state::render(const mce::core::frame_time&) {
	auto sys = static_cast<renderer_system*>(system_);
	auto& frame_data = sys->per_frame_data();
	if(camera_comps.empty()) return;
	collect_scene_uniforms();
	auto scene_uniform_descriptor = frame_data.uniform_buffer.store(scene_uniforms);
	frame_data.scene_descriptor_set =
			frame_data.discriptor_pool.allocate_descriptor_set(sys->descriptor_set_layout_per_scene_);
	frame_data.scene_descriptor_set.get().update()(0, 0, vk::DescriptorType::eUniformBuffer,
												   {scene_uniform_descriptor});
	for(renderer_system::per_frame_per_thread_data_t& local_data :
		sys->per_frame_per_thread_data_[sys->gs_.current_swapchain_image()].all()) {
		record_per_scene_data(local_data, frame_data);
	}
	task_reducer red(*this);
	tbb::parallel_reduce(containers::make_pool_const_range(static_model_comps), red);
	tbb::parallel_sort(*(red.buffer));
	using range = tbb::blocked_range<decltype(red.buffer->begin())>;
	tbb::parallel_for(range(red.buffer->begin(), red.buffer->end()), [this, sys](const range& r) {
		// auto& per_thread_data = sys->per_thread_data();
		auto& per_frame_per_thread_data = sys->per_frame_per_thread_data();
		util::grouped_foreach(
				r.begin(), r.end(),
				[this, &per_frame_per_thread_data](const render_task& task) {
					record_render_task(task, per_frame_per_thread_data);
				},
				util::make_foreach_grouping(
						[](const render_task& a, const render_task& b) {
							return a.used_material == b.used_material;
						},
						[this, &per_frame_per_thread_data](const render_task& task) {
							record_per_material_data(task.used_material, per_frame_per_thread_data);
						},
						[](const render_task&) {}),
				util::make_foreach_grouping(
						[](const render_task& a, const render_task& b) { return a.used_mesh == b.used_mesh; },
						[this, &per_frame_per_thread_data](const render_task& task) {
							record_per_mesh_data(task.used_mesh, per_frame_per_thread_data);
						},
						[](const render_task&) {}));
	});
}
void renderer_state::task_reducer::
operator()(const containers::smart_object_pool_range<
		   containers::smart_object_pool<static_model_component>::const_iterator>& range) {
	for(const static_model_component& c : range) {
		if(c.ready()) {
			assert(c.model());
			assert(c.materials().size() == c.model()->meshes().size());
			for(size_t i = 0; i < c.model()->meshes().size(); ++i) {
				const auto& mesh = c.model()->meshes()[i];
				auto mat = c.materials()[i].get();
				auto transform = c.owner().calculate_transform();
				buffer->push_back(render_task{mat, &mesh, {transform}});
			}
		}
	}
}
void renderer_state::task_reducer::join(const task_reducer& other) {
	buffer->insert(buffer->end(), other.buffer->begin(), other.buffer->end());
}

} /* namespace rendering */
} /* namespace mce */
