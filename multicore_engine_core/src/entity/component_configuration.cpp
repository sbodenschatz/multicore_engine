/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/component_configuration.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <iterator>
#include <mce/containers/smart_pool_ptr.hpp>
#include <mce/entity/component_configuration.hpp>
#include <mce/entity/component_property_assignment.hpp>
#include <mce/entity/component_type.hpp>
#include <mce/exceptions.hpp>
#include <mce/reflection/property.hpp>
#include <memory>
#include <string>

namespace mce {
namespace entity {

component_configuration::component_configuration(core::engine* engine, const abstract_component_type& type)
		: engine(engine), type_(type) {}

component_configuration::component_configuration(const component_configuration& other)
		: engine(other.engine), type_(other.type_) {
	assignments.reserve(other.assignments.size());
	std::transform(other.assignments.begin(), other.assignments.end(), std::back_inserter(assignments),
				   [](const auto& assignment) { return assignment->make_copy(); });
}

component_pool_ptr component_configuration::create_component(entity& owner) const {
	component_pool_ptr comp = type_.create_component(owner, *this, engine);
	for(const auto& assignment : assignments) {
		assignment->assign(*comp);
	}
	return comp;
}

component_configuration::~component_configuration() {}

void component_configuration::make_assignment(const std::string& property_name,
											  const ast::variable_value& ast_value,
											  const std::string& entity_context,
											  entity_manager& entity_manager) {
	auto it = std::find_if(assignments.begin(), assignments.end(), [&](const auto& elem) {
		return elem->abstract_property().name() == property_name;
	});
	if(it == assignments.end()) {
		auto it2 = std::find_if(type_.properties().begin(), type_.properties().end(),
								[&](const auto& elem) { return elem->name() == property_name; });
		if(it2 == type_.properties().end()) {
			if(type_.takes_unbound_property_values()) {
				unbound_property_values_[property_name] = ast_value;
				return;
			} else {
				throw invalid_property_exception("Unknown property '" + property_name +
												 "' of component type '" + type_.name() + "'.");
			}
		}
		it = assignments.emplace(assignments.end(), it2->get()->make_assignment(engine));
	}
	it->get()->parse(ast_value, entity_context, type_.name(), entity_manager);
}

} // namespace entity
} // namespace mce
