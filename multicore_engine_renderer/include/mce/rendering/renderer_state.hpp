/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/renderer_state.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_RENDERER_STATE_HPP_
#define MCE_RENDERING_RENDERER_STATE_HPP_

/**
 * \file
 * Defines the renderer_state class.
 */

#include <glm/matrix.hpp>
#include <mce/containers/scratch_pad_pool.hpp>
#include <mce/containers/smart_object_pool.hpp>
#include <mce/containers/smart_object_pool_range.hpp>
#include <mce/containers/smart_pool_ptr.hpp>
#include <mce/core/system_state.hpp>
#include <mce/memory/aligned_new.hpp>
#include <mce/rendering/camera_component.hpp>
#include <mce/rendering/point_light_component.hpp>
#include <mce/rendering/static_model.hpp>
#include <mce/rendering/static_model_component.hpp>
#include <tbb/parallel_reduce.h>

namespace mce {
namespace entity {
class entity_manager;
} // namespace entity
namespace rendering {

/// Provides the game_state specific data management for the renderer_system.
/**
 * Keeps the component objects for the renderer_system bound to the entities in the state to which the
 * renderer_state is bound.
 */
class renderer_state : public core::system_state {
	containers::smart_object_pool<camera_component, 64> camera_comps;
	containers::smart_object_pool<point_light_component> point_light_comps;
	containers::smart_object_pool<static_model_component> static_model_comps;

	struct render_task {
		material* used_material;
		static_model::mesh* used_mesh;
		glm::mat4 transform;
	};

	containers::scratch_pad_pool<std::vector<render_task>> render_task_buffer_pool;

	struct task_reducer {
		renderer_state& rs;
		containers::scratch_pad_pool<std::vector<render_task>>::object buffer;
		task_reducer(renderer_state& rs) : rs{rs}, buffer{rs.render_task_buffer_pool.get()} {}
		task_reducer(const task_reducer& other, tbb::split)
				: rs{other.rs}, buffer{rs.render_task_buffer_pool.get()} {}
		void operator()(const containers::smart_object_pool_range<
						containers::smart_object_pool<static_model_component>::const_iterator>& range);
		void join(const task_reducer& other);
	};

public:
	ALIGNED_NEW_AND_DELETE(renderer_state)

	/// Creates the renderer_state for the given system (must be a pointer to a renderer_system object).
	/**
	 * Should be called by core::game_state::add_system_state.
	 */
	explicit renderer_state(core::system* sys);
	/// Destroys the renderer_state and releases the used resources.
	~renderer_state();

	/// Creates a camera_component for the given entity and using the given configuration.
	containers::smart_pool_ptr<camera_component>
	create_camera_component(entity::entity& owner, const entity::component_configuration& configuration) {
		return camera_comps.emplace(owner, configuration);
	}
	/// Creates a point_light_component for the given entity and using the given configuration.
	containers::smart_pool_ptr<point_light_component>
	create_point_light_component(entity::entity& owner,
								 const entity::component_configuration& configuration) {
		return point_light_comps.emplace(owner, configuration);
	}
	/// Creates a static_model_component for the given entity and using the given configuration.
	containers::smart_pool_ptr<static_model_component>
	create_static_model_component(renderer_state& sys, entity::entity& owner,
								  const entity::component_configuration& configuration) {
		return static_model_comps.emplace(sys, owner, configuration);
	}

	/// Registers the component types managed by renderer_state to the given entity_manager object.
	void register_to_entity_manager(entity::entity_manager& em);

	void render(const mce::core::frame_time& frame_time) override;
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_RENDERER_STATE_HPP_ */
