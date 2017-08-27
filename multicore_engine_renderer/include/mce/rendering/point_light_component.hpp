/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/point_light_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_POINT_LIGHT_COMPONENT_HPP_
#define MCE_RENDERING_POINT_LIGHT_COMPONENT_HPP_

#include <mce/entity/component.hpp>

namespace mce {
namespace entity {
class entity;
class component_configuration;
} // namespace entity
namespace rendering {

class point_light_component : public entity::component {
public:
	point_light_component(entity::entity& owner, entity::component_configuration& conf);
	~point_light_component();
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_POINT_LIGHT_COMPONENT_HPP_ */
