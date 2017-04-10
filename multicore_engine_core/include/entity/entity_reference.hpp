/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/entity_reference.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_ENTITY_REFERENCE_HPP_
#define ENTITY_ENTITY_REFERENCE_HPP_

#include <string>

namespace mce {
namespace entity {
class entity_manager;
class entity;

class entity_reference {
private:
	std::string referenced_entity_name_;
	const entity_manager* entity_manager_;

public:
	// cppcheck-suppress passedByValue
	entity_reference(std::string referenced_entity_name, const entity_manager& entity_manager)
			: referenced_entity_name_{std::move(referenced_entity_name)}, entity_manager_{&entity_manager} {}

	entity_reference() : referenced_entity_name_{""}, entity_manager_{nullptr} {}

	entity* resolve() const;

	const entity_manager* entity_manager() const {
		return entity_manager_;
	}

	const std::string& referenced_entity_name() const {
		return referenced_entity_name_;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_ENTITY_REFERENCE_HPP_ */
