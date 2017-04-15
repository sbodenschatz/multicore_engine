/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/entity.cpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#include <boost/container/vector.hpp>
#include <bstream/ibstream.hpp>
#include <bstream/obstream.hpp>
#include <entity/component.hpp>
#include <entity/component_configuration.hpp>
#include <entity/component_type.hpp>
#include <entity/entity.hpp>
#include <entity/entity_manager.hpp>
#include <exceptions.hpp>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>

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

void entity::store_to_bstream(bstream::obstream& ostr) const {
	ostr << position_;
	ostr << orientation_;
	ostr << uint32_t(components_.size());
	for(const auto& comp : components_) {
		ostr << comp.first;
	}
	for(const auto& comp : components_) {
		comp.second->store_to_bstream(ostr);
	}
}
void entity::load_from_bstream(bstream::ibstream& istr, const entity_manager& ent_mgr, core::engine* engine) {
	istr >> position_;
	istr >> orientation_;
	component_container<component_type_id_t> loaded_component_ids;
	uint32_t comp_count;
	istr >> comp_count;
	for(uint32_t i = 0; i < comp_count; ++i) {
		component_type_id_t id;
		istr >> id;
		loaded_component_ids.push_back(id);
	}
	component_container<component_type_id_t> current_component_ids;
	component_container<component_type_id_t> created_component_ids;
	component_container<component_type_id_t> removed_component_ids;
	using comp_elem_t = std::pair<component_type_id_t, containers::smart_pool_ptr<mce::entity::component>>;
	std::transform(components_.begin(), components_.end(), std::back_inserter(current_component_ids),
				   [](const comp_elem_t& e) { return e.first; });
	std::set_difference(loaded_component_ids.begin(), loaded_component_ids.end(),
						current_component_ids.begin(), current_component_ids.end(),
						std::back_inserter(created_component_ids));
	std::set_difference(current_component_ids.begin(), current_component_ids.end(),
						loaded_component_ids.begin(), loaded_component_ids.end(),
						std::back_inserter(removed_component_ids));
	for(component_type_id_t id : removed_component_ids) {
		components_.erase(id);
	}
	for(component_type_id_t id : created_component_ids) {
		auto comp_type = ent_mgr.find_component_type(id);
		if(!comp_type)
			throw invalid_component_type_exception("Unknown component_type id " + std::to_string(id) + ".");
		components_.insert(id, comp_type->create_component(*this, comp_type->empty_configuration(), engine));
	}
	for (auto& comp : components_) {
		comp.second->load_from_bstream(istr);
	}
}

} // namespace entity
} // namespace mce
