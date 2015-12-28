/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity_type.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <iterator>
#include <algorithm>
#include <entity/entity_configuration.hpp>
#include <entity/component_configuration.hpp>
#include <entity/entity.hpp>

namespace mce {
namespace entity {

entity_configuration::entity_configuration(const std::string& name) : name_(name) {}

entity_configuration::entity_configuration(const entity_configuration& other) : name_(other.name_) {
	std::transform(other.components_.begin(), other.components_.end(), std::back_inserter(components_),
				   [](const auto& comp) { return std::make_unique<component_configuration>(*comp); });
}

entity_configuration::~entity_configuration() {}

void entity_configuration::create_components(entity& entity) const {
	for(const auto& comp : components_) {
		entity.add_component(comp->create_component(entity));
	}
}

} // namespace entity
} // namespace mce
