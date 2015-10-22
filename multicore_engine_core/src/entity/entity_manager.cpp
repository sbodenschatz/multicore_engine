/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity_manager.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <entity/entity_manager.hpp>
#include <entity/entity_configuration.hpp>
#include <entity/component_type.hpp>

namespace mce {
namespace entity {

entity_manager::entity_manager(core::engine& engine) : engine(engine) {
	register_builtin_components();
}

entity_manager::~entity_manager() {}

void entity_manager::register_builtin_components() {}

} // namespace entity
} // namespace mce
