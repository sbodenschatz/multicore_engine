/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/component_configuration.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <entity/component_configuration.hpp>
#include <entity/component_type.hpp>

namespace mce {
namespace entity {

component_configuration::component_configuration(
		abstract_component_type& type, const std::unordered_map<std::string, std::string>& property_values)
		: type_(type) {
	for(const auto& prop : type.properties()) {
		auto assignment = prop->make_assignment();
		auto& prop_name = prop->name();
		auto it = property_values.find(prop_name);
		if(it != property_values.end()) {
			assignment->parse(it->second);
			assignments.push_back(std::move(assignment));
		}
	}
}

component_pool_ptr component_configuration::create_component(entity& owner, core::engine& engine) const {
	component_pool_ptr comp = type_.create_component(owner, *this, engine);
	for(const auto& assignment : assignments) { assignment->assign(*comp); }
	return comp;
}

} // namespace entity
} // namespace mce
