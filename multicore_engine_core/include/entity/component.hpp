/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_HPP_
#define ENTITY_COMPONENT_HPP_

#include <vector>
#include <memory>
#include <reflection/property.hpp>

namespace mce {
namespace entity {

class entity;
class component_configuration;

class component {
private:
	entity& owner_;
	const component_configuration& configuration_;

protected:
	component(entity& owner, component_configuration& configuration) noexcept
			: owner_(owner),
			  configuration_(configuration) {}

public:
	typedef std::vector<std::unique_ptr<reflection::abstract_property<component>>> property_list;
	virtual ~component() = default;

	const component_configuration& configuration() const {
		return configuration_;
	}

	const entity& owner() const {
		return owner_;
	}

	entity& owner() {
		return owner_;
	}

	static void fill_property_list(property_list& properties);
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_HPP_ */
