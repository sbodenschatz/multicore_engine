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

class component {
private:
	entity& owner;

protected:
	component(entity& owner) noexcept : owner(owner) {}

public:
	virtual ~component() = default;
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_HPP_ */
