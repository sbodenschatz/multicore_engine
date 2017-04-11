/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity_reference.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <entity/entity_manager.hpp>
#include <entity/entity_reference.hpp>

namespace mce {
namespace entity {

entity* entity_reference::resolve() const {
	if(!entity_manager_) return nullptr;
	return entity_manager_->find_entity(referenced_entity_name_);
}

} // namespace entity
} // namespace mce
