/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/point_light_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_POINT_LIGHT_COMPONENT_HPP_
#define MCE_RENDERING_POINT_LIGHT_COMPONENT_HPP_

#include <glm/vec3.hpp>
#include <mce/entity/component.hpp>

namespace mce {
namespace entity {
class entity;
class component_configuration;
} // namespace entity
namespace rendering {

class point_light_component : public entity::component {
private:
	glm::vec3 color_;
	float radius_;

public:
	point_light_component(entity::entity& owner, entity::component_configuration& conf);
	~point_light_component();

	glm::vec3 color() const {
		return color_;
	}

	void color(const glm::vec3& color) {
		color_ = color;
	}

	float radius() const {
		return radius_;
	}

	void radius(float radius) {
		radius_ = radius;
	}

	static void fill_property_list(property_list& prop);
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_POINT_LIGHT_COMPONENT_HPP_ */
