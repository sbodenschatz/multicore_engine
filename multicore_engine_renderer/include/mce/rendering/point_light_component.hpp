/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/point_light_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_POINT_LIGHT_COMPONENT_HPP_
#define MCE_RENDERING_POINT_LIGHT_COMPONENT_HPP_

/**
 * \file
 * Defines the point_light_component class.
 */

#include <glm/vec3.hpp>
#include <mce/entity/component.hpp>

namespace mce {
namespace entity {
class entity;
class component_configuration;
} // namespace entity
namespace rendering {

/// Makes the entity it is attached to act as a point light in the scene.
class point_light_component : public entity::component {
private:
	glm::vec3 color_;
	float radius_;

public:
	/// \brief Creates a point_light_component for the given entity to attach to and the given
	/// component_configuration from which properties will be initialized..
	point_light_component(entity::entity& owner, const entity::component_configuration& conf);
	/// Destroys the point_light_component.
	~point_light_component();

	/// Returns the color of the emitted light.
	glm::vec3 color() const {
		return color_;
	}

	/// Changes the color of the emitted light.
	void color(const glm::vec3& color) {
		color_ = color;
	}

	/// Returns the influence radius of the light.
	float radius() const {
		return radius_;
	}

	/// Changes the influence radius of the light.
	void radius(float radius) {
		radius_ = radius;
	}

	/// Fills the given property_list with the properties available for this class.
	static void fill_property_list(property_list& prop);
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_POINT_LIGHT_COMPONENT_HPP_ */
