/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/renderer_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <cassert>
#include <glm/gtc/matrix_transform.hpp>
#include <mce/entity/entity_manager.hpp>
#include <mce/rendering/renderer_state.hpp>

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

void renderer_state::render(const mce::core::frame_time&) {
	task_reducer red(*this);
	tbb::parallel_reduce(containers::make_pool_const_range(static_model_comps), red);
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
				buffer->push_back(render_task{mat, &mesh, transform});
			}
		}
	}
}
void renderer_state::task_reducer::join(const task_reducer& other) {
	buffer->insert(buffer->end(), other.buffer->begin(), other.buffer->end());
}

} /* namespace rendering */
} /* namespace mce */
