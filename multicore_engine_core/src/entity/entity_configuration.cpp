/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity_type.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <entity/entity_configuration.hpp>
#include <entity/component_configuration.hpp>
#include <entity/entity.hpp>

namespace mce {
namespace entity {

entity_configuration::entity_configuration(const std::string& name) : name_(name) {}

void entity_configuration::create_components(entity& entity, core::engine& engine) const {
	for(const auto& comp : components_) { entity.add_component(comp->create_component(entity, engine)); }
}

} // namespace entity
} // namespace mce
