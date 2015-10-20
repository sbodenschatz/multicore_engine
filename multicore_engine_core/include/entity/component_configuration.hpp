/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_configuration.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_CONFIGURATION_HPP_
#define ENTITY_COMPONENT_CONFIGURATION_HPP_

#include <unordered_map>
#include <vector>
#include "ecs_types.hpp"
#include <reflection/property_assignment.hpp>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace entity {
class entity;
class abstract_component_type;

class component_configuration {
	abstract_component_type& type_;
	std::vector<std::unique_ptr<reflection::abstract_property_assignment<component>>> assignments;

public:
	explicit component_configuration(abstract_component_type& type,
									 const std::unordered_map<std::string, std::string>& property_values);
	component_pool_ptr create_component(entity& owner, core::engine& engine) const;

	const abstract_component_type& type() const {
		return type_;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_CONFIGURATION_HPP_ */
