/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/rendering/point_light_component.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/rendering/point_light_component.hpp>

namespace mce {
namespace rendering {

point_light_component::point_light_component(entity::entity& owner, entity::component_configuration& conf)
		: component(owner, conf), color_(1.0f, 1.0f, 1.0f), radius_(1000.0f) {}

point_light_component::~point_light_component() {}

} /* namespace rendering */
} /* namespace mce */
