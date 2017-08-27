/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/rendering/static_model_component.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_RENDERING_STATIC_MODEL_COMPONENT_HPP_
#define MCE_RENDERING_STATIC_MODEL_COMPONENT_HPP_

#include <mce/entity/component.hpp>

namespace mce {
namespace entity {
class entity;
class component_configuration;
} // namespace entity
namespace rendering {

class static_model_component : public entity::component {
public:
	static_model_component(entity::entity& owner, entity::component_configuration& conf);
	~static_model_component();
};

} /* namespace rendering */
} /* namespace mce */

#endif /* MCE_RENDERING_STATIC_MODEL_COMPONENT_HPP_ */
