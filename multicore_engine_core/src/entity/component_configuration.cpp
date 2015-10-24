/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/component_configuration.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <iterator>
#include <algorithm>
#include <entity/component_configuration.hpp>
#include <entity/component_type.hpp>
#include <reflection/property_assignment.hpp>

namespace mce {
namespace entity {

component_configuration::component_configuration(
		abstract_component_type& type, const std::unordered_map<std::string, std::string>& property_values)
		: type_(type) {
	// TODO:Replace with AST implementation
	//	for(const auto& prop : type.properties()) {
	//		auto assignment = prop->make_assignment();
	//		auto& prop_name = prop->name();
	//		auto it = property_values.find(prop_name);
	//		if(it != property_values.end()) {
	//			assignment->parse(it->second);
	//			assignments.push_back(std::move(assignment));
	//		}
	//	}
}

component_configuration::component_configuration(const component_configuration& other) : type_(other.type_) {
	std::transform(other.assignments.begin(), other.assignments.end(), std::back_inserter(assignments),
				   [](const auto& assignment) { return assignment->make_copy(); });
}

component_pool_ptr component_configuration::create_component(entity& owner, core::engine& engine) const {
	component_pool_ptr comp = type_.create_component(owner, *this, engine);
	for(const auto& assignment : assignments) { assignment->assign(*comp); }
	return comp;
}

component_configuration::~component_configuration() {}

} // namespace entity
} // namespace mce
