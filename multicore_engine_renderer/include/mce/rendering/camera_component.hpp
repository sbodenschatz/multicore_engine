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
public:
	camera_component(entity::entity& owner, entity::component_configuration& conf);
	~camera_component();
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_CAMERA_COMPONENT_HPP_ */
