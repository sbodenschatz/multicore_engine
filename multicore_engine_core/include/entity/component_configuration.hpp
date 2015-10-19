/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_configuration.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_CONFIGURATION_HPP_
#define ENTITY_COMPONENT_CONFIGURATION_HPP_

#include "ecs_types.hpp"

namespace mce {
namespace core {
class engine;
} // namespace core
namespace entity {
class entity;
class abstract_component_type;

class component_configuration {
	abstract_component_type& type_;

public:
	explicit component_configuration(abstract_component_type& type) : type_(type) {}
	component_pool_ptr create_component(entity& owner, core::engine& engine) const;

	const abstract_component_type& type() const {
		return type_;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_CONFIGURATION_HPP_ */
