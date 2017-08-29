/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/renderer_state.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/entity/entity_manager.hpp>
#include <mce/rendering/renderer_state.hpp>

namespace mce {
namespace rendering {

renderer_state::renderer_state(core::system* sys) : system_state(sys) {}

renderer_state::~renderer_state() {}

void renderer_state::register_with_entity_manager(entity::entity_manager& em) {
	REGISTER_COMPONENT_TYPE_SIMPLE(em, camera, this->create_camera_component(owner, config), this);
	REGISTER_COMPONENT_TYPE_SIMPLE(em, point_light, this->create_point_light_component(owner, config), this);
	REGISTER_COMPONENT_TYPE_SIMPLE(em, static_model,
								   this->create_static_model_component(*this, owner, config), this);
}

} /* namespace rendering */
} /* namespace mce */
