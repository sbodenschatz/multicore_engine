/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/entity_configuration.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_ENTITY_CONFIGURATION_HPP_
#define ENTITY_ENTITY_CONFIGURATION_HPP_

#include <memory>
#include <string>
#include <vector>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace entity {
class entity;
class component_configuration;

/// Represents a configuration (consisting of component configurations) from which entity objects are created.
class entity_configuration {
	std::string name_;
	std::vector<std::unique_ptr<component_configuration>> components_;
	void create_components(entity& entity) const;

public:
	friend class entity_manager;
	/// Constructs an entity configuration with the given name.
	explicit entity_configuration(const std::string& name);
	/// Allows copy-construction of entity_configuration.
	entity_configuration(const entity_configuration& other);
	/// Allows move-construction of entity_configuration.
	entity_configuration(entity_configuration&&) = default;
	/// Forbids copy-assignment of entity_configuration.
	entity_configuration& operator=(const entity_configuration&) = delete;
	/// Forbids move-assignment of entity_configuration.
	entity_configuration& operator=(entity_configuration&&) = delete;
	/// Destroys the entity_configuration.
	~entity_configuration();
	/// Allows access to the container containing the component configuration in this entity_configuration.
	const std::vector<std::unique_ptr<component_configuration>>& components() const {
		return components_;
	}
	/// Allows access to the container containing the component configuration in this entity_configuration.
	std::vector<std::unique_ptr<component_configuration>>& components() {
		return components_;
	}
	/// Returns the name of the entity_configuration.
	const std::string& name() const {
		return name_;
	}
	/// Sets the name of the entity_configuration.
	void name(const std::string& value) {
		name_ = value;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_ENTITY_CONFIGURATION_HPP_ */
