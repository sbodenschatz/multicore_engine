/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/entity_configuration.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_ENTITY_CONFIGURATION_HPP_
#define ENTITY_ENTITY_CONFIGURATION_HPP_

#include <string>
#include <vector>
#include <memory>

namespace mce {
namespace core {
class engine;
} // namespace core
namespace entity {
class entity;
class component_configuration;

class entity_configuration {
	std::string name_;
	std::vector<std::unique_ptr<component_configuration>> components_;
	void create_components(entity& entity, core::engine& engine) const;

public:
	explicit entity_configuration(const std::string& name);
	entity_configuration(const entity_configuration& other);
	entity_configuration(entity_configuration&&) = default;
	entity_configuration& operator=(const entity_configuration&) = delete;
	entity_configuration& operator=(entity_configuration&&) = delete;
	~entity_configuration();

	const std::vector<std::unique_ptr<component_configuration>>& components() const {
		return components_;
	}

	const std::string& name() const {
		return name_;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_ENTITY_CONFIGURATION_HPP_ */
