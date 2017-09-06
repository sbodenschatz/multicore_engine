/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/camera_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_CAMERA_COMPONENT_HPP_
#define MCE_RENDERING_CAMERA_COMPONENT_HPP_

/**
 * \file
 * Defines the camera_component class.
 */

#include <mce/entity/component.hpp>

namespace mce {
namespace entity {
class entity;
class component_configuration;
} // namespace entity
namespace rendering {

/// \brief Adds camera functionality to the entity it is attached to, meaning that the scene can be rendered
/// from the perspective of the entity.
class camera_component : public entity::component {
	float fov_;
	float near_plane_;
	float far_plane_;

public:
	/// \brief Creates a camera_component for the given entity to attach to and the given
	/// component_configuration from which properties will be initialized.
	camera_component(entity::entity& owner, const entity::component_configuration& conf);
	/// Destroys the camera component.
	~camera_component();

	/// Returns the distance from the camera view point to the far clipping plane.
	float far_plane() const {
		return far_plane_;
	}

	/// Changes the distance from the camera view point to the far clipping plane.
	void far_plane(float far_plane) {
		far_plane_ = far_plane;
	}

	/// Returns the field-of-view angle (the opening angle of the view frustum) in degrees.
	float fov() const {
		return fov_;
	}

	/// Changes the field-of-view angle (the opening angle of the view frustum) in degrees.
	void fov(float fov) {
		fov_ = fov;
	}

	/// Returns the distance from the camera view point to the far clipping plane.
	float near_plane() const {
		return near_plane_;
	}

	/// Changes the distance from the camera view point to the far clipping plane.
	void near_plane(float near_plane) {
		near_plane_ = near_plane;
	}

	/// Fills the given property_list with the properties available for this class.
	static void fill_property_list(property_list& prop);
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_CAMERA_COMPONENT_HPP_ */
