/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity_type.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <iterator>
#include <mce/entity/component_configuration.hpp>
#include <mce/entity/entity.hpp>
#include <mce/entity/entity_configuration.hpp>

namespace mce {
namespace entity {

entity_configuration::entity_configuration(const std::string& name) : name_(name) {}

entity_configuration::entity_configuration(const entity_configuration& other) : name_(other.name_) {
	components_.reserve(other.components_.size());
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
