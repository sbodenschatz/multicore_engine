/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_HPP_
#define ENTITY_COMPONENT_HPP_

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
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_HPP_ */
