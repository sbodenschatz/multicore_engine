/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/camera_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_CAMERA_COMPONENT_HPP_
#define MCE_RENDERING_CAMERA_COMPONENT_HPP_

#include <mce/entity/component.hpp>

namespace mce {
namespace entity {
class entity;
class component_configuration;
} // namespace entity
namespace rendering {

class camera_component : public entity::component {
	float fov_;
	float near_plane_;
	float far_plane_;

public:
	camera_component(entity::entity& owner, entity::component_configuration& conf);
	~camera_component();

	float far_plane() const {
		return far_plane_;
	}

	void far_plane(float far_plane) {
		far_plane_ = far_plane;
	}

	float fov() const {
		return fov_;
	}

	void fov(float fov) {
		fov_ = fov;
	}

	float near_plane() const {
		return near_plane_;
	}

	void near_plane(float near_plane) {
		near_plane_ = near_plane;
	}

	static void fill_property_list(property_list& prop);
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_CAMERA_COMPONENT_HPP_ */
