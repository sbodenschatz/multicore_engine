/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/entity/component_configuration.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_CONFIGURATION_HPP_
#define ENTITY_COMPONENT_CONFIGURATION_HPP_

/**
 * \file
 * Defines the component_configuration class.
 */

#include <boost/container/flat_map.hpp>
#include <mce/entity/component.hpp>
#include <mce/entity/component_property_assignment.hpp>
#include <mce/entity/ecs_types.hpp>
#include <mce/entity/parser/entity_text_file_ast.hpp>
#include <memory>
#include <string>
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
class entity_manager;

/// Represents the configuration of a component in an entity_configuration.
/**
 * The presence of an object of this class in an entity_configuration specifies that an entity created from
 * that entity_configuration contains a component of the stored type and with the stored component property
 * assignments to parameterize the component.
 */
class component_configuration {
	core::engine* engine;
	const abstract_component_type& type_;
	std::vector<std::unique_ptr<abstract_component_property_assignment<component>>> assignments;
	boost::container::flat_map<std::string, ast::variable_value> unbound_property_values_;

public:
	/// \brief Creates a component_configuration for the given engine object and with the given
	/// abstract_component_type.
	component_configuration(core::engine* engine, const abstract_component_type& type);
	/// Allows copy-construction for component_configuration.
	component_configuration(const component_configuration& other);
	/// Allows move-construction for component_configuration.
	component_configuration(component_configuration&&) = default;
	/// Forbids copy-assignment for component_configuration.
	component_configuration& operator=(const component_configuration&) = delete;
	/// Forbids move-assignment for component_configuration.
	component_configuration& operator=(component_configuration&&) = delete;
	/// Destroys the component_configuration object.
	~component_configuration();
	/// Creates a component object for the given entity with the configuration represented by this object.
	component_pool_ptr create_component(entity& owner) const;
	/// \brief Adds a property assignment for the named property with the given value using the given
	/// entity_context for error messages.
	void make_assignment(const std::string& property_name, const ast::variable_value& ast_value,
						 const std::string& entity_context, entity_manager& entity_manager);

	/// Returns the type of the component this configuration specifies.
	const abstract_component_type& type() const {
		return type_;
	}

	/// Allows access to property values that are not consumed by the bound reflection properties.
	/**
	 * This is only filled if the takes_unbound_property_values() static function of the component class
	 * returns true. Otherwise additional properties given cause an exception.
	 */
	const boost::container::flat_map<std::string, ast::variable_value>& unbound_property_values() const {
		return unbound_property_values_;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_CONFIGURATION_HPP_ */
