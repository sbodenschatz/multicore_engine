/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_type.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_TYPE_HPP_
#define ENTITY_COMPONENT_TYPE_HPP_

#include <core/engine.hpp>
#include <entity/component.hpp>
#include <entity/component_configuration.hpp>
#include <entity/component_property_assignment.hpp>
#include <entity/component_type_id_manager.hpp>
#include <entity/ecs_types.hpp>
#include <reflection/property.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace mce {
namespace core {
class engine;
} // namespace core

namespace entity {
class entity;

/// \brief Represents an abstract base class for component type descriptions to allow inserting them into a
/// polymorphic container.
class abstract_component_type {
public:
	/// Specifies the type of the list of properties.
	typedef std::vector<std::unique_ptr<reflection::abstract_property<
			component, abstract_component_property_assignment, core::engine&>>> property_list;

private:
	component_type_id_t id_;
	std::string name_;
	component_configuration empty_configuration_;

protected:
	/// Stores the list of properties registered for the component type.
	property_list properties_;
	/// Allows implementing classes to construct the base class with the given type id and name.
	// cppcheck-suppress passedByValue
	abstract_component_type(core::engine& engine, component_type_id_t id, std::string name)
			: id_(id), name_(std::move(name)), empty_configuration_(engine, *this) {}

public:
	/// Forbids copy-construction of abstract_component_type.
	abstract_component_type(const abstract_component_type&) = delete;
	/// Forbids move-construction of abstract_component_type.
	abstract_component_type(abstract_component_type&&) = delete;
	/// Forbids copy-assignment of abstract_component_type.
	abstract_component_type& operator=(const abstract_component_type&) = delete;
	/// Forbids move-assignment of abstract_component_type.
	abstract_component_type& operator=(abstract_component_type&&) = delete;
	/// Enables virtual destruction for implementing classes.
	virtual ~abstract_component_type() = default;
	/// \brief Creates a component object of the type described by this component_type for the given owner
	/// entity, component_configuration and engine object.
	virtual component_pool_ptr create_component(entity& owner, const component_configuration& config,
												core::engine& engine) const = 0;
	/// Provides access to the list of properties registered for the described type.
	const property_list& properties() const noexcept {
		return properties_;
	}
	/// Returns the id of the component type.
	component_type_id_t id() const noexcept {
		return id_;
	}
	/// Returns the name of the component type.
	const std::string& name() const noexcept {
		return name_;
	}
	/// Returns an empty component_configuration for this component_type.
	const component_configuration& empty_configuration() const noexcept {
		return empty_configuration_;
	}
};

/// \brief Represents a concrete description of a component type with a specific type T of the component and
/// function object type F that constructs an object of T.
template <typename T, typename F>
class component_type : public abstract_component_type {
	F factory_function_;

public:
	/// \brief Constructs a component_type description for T with the given name and component object factory
	/// function.
	component_type(core::engine& engine, const std::string& name, const F& factory_function)
			: abstract_component_type(engine, component_type_id_manager::id<T>(), name),
			  factory_function_(factory_function) {
		T::fill_property_list(properties_);
	}
	/// Destroys the component_type object.
	virtual ~component_type() override = default;
	/// \brief Creates a component object of the type described by this component_type for the given owner
	/// entity, component_configuration and engine object.
	virtual component_pool_ptr create_component(entity& owner, const component_configuration& config,
												core::engine& engine) const override {
		return factory_function_(owner, config, engine);
	}
};

/// Create a component type description for T with the given name and factory function.
template <typename T, typename F>
std::unique_ptr<abstract_component_type> make_component_type(core::engine& engine, const std::string& name,
															 const F& factory_function) {
	return std::make_unique<component_type<T, F>>(engine, name, factory_function);
}

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_TYPE_HPP_ */
