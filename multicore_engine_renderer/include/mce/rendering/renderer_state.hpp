/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/renderer_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERER_STATE_HPP_
#define MCE_RENDERING_RENDERER_STATE_HPP_

#include <mce/containers/smart_object_pool.hpp>
#include <mce/containers/smart_pool_ptr.hpp>
#include <mce/core/system_state.hpp>
#include <mce/memory/aligned_new.hpp>
#include <mce/rendering/camera_component.hpp>
#include <mce/rendering/point_light_component.hpp>
#include <mce/rendering/static_model_component.hpp>

namespace mce {
namespace entity {
class entity_manager;
} // namespace entity
namespace rendering {

class renderer_state : public core::system_state {
	containers::smart_object_pool<camera_component, 64> camera_comps;
	containers::smart_object_pool<point_light_component> point_light_comps;
	containers::smart_object_pool<static_model_component> static_model_comps;

public:
	ALIGNED_NEW_AND_DELETE(renderer_state)

	explicit renderer_state(core::system* sys);
	~renderer_state();

	containers::smart_pool_ptr<camera_component>
	create_camera_component(entity::entity& owner, const entity::component_configuration& configuration) {
		return camera_comps.emplace(owner, configuration);
	}
	containers::smart_pool_ptr<point_light_component>
	create_point_light_component(entity::entity& owner,
								 const entity::component_configuration& configuration) {
		return point_light_comps.emplace(owner, configuration);
	}
	containers::smart_pool_ptr<static_model_component>
	create_static_model_component(renderer_state& sys, entity::entity& owner,
								  const entity::component_configuration& configuration) {
		return static_model_comps.emplace(sys, owner, configuration);
	}

	void register_to_entity_manager(entity::entity_manager& em);
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_STATE_HPP_ */
