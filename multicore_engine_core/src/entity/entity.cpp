/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <entity/entity.hpp>
#include <entity/component.hpp>
#include <entity/component_configuration.hpp>
#include <entity/component_type.hpp>

namespace mce {
namespace entity {

const mce::entity::component* entity::component(component_type_id_t id) const {
	auto it = components_.find(id);
	if(it != components_.end()) {
		return it->second.get();
	} else {
		return nullptr;
	}
}
mce::entity::component* entity::component(component_type_id_t id) {
	auto it = components_.find(id);
	if(it != components_.end()) {
		return it->second.get();
	} else {
		return nullptr;
	}
}
bool entity::has_component(component_type_id_t id) const {
	auto it = components_.find(id);
	return it != components_.end();
}

void entity::add_component(component_pool_ptr&& comp) {
	component_type_id_t id = comp->configuration().type().id();
	bool success = false;
	std::tie(std::ignore, success) = components_.insert(id, std::move(comp));
	if(!success) throw std::invalid_argument("Component of this type is already present at this entity.");
}

} // namespace entity
} // namespace mce
