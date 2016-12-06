/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_configuration.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_CONFIGURATION_HPP_
#define ENTITY_COMPONENT_CONFIGURATION_HPP_

#include "ecs_types.hpp"
#include "parser/entity_text_file_ast.hpp"
#include <unordered_map>
#include <vector>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace entity {
template <typename Root_Type>
class abstract_component_property_assignment;
class entity;
class abstract_component_type;

class component_configuration {
	core::engine& engine;
	const abstract_component_type& type_;
	std::vector<std::unique_ptr<abstract_component_property_assignment<component>>> assignments;

public:
	component_configuration(core::engine& engine, const abstract_component_type& type);
	component_configuration(const component_configuration& other);
	component_configuration(component_configuration&&) = default;
	component_configuration& operator=(const component_configuration&) = delete;
	component_configuration& operator=(component_configuration&&) = delete;
	~component_configuration();
	component_pool_ptr create_component(entity& owner) const;
	void make_assignment(const std::string& property_name, const ast::variable_value& ast_value,
						 const std::string& entity_context);

	const abstract_component_type& type() const {
		return type_;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_CONFIGURATION_HPP_ */
