/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/entity/component_configuration.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <entity/component_configuration.hpp>
#include <entity/component_type.hpp>

namespace mce {
namespace entity {

component_pool_ptr component_configuration::create_component(entity& owner, core::engine& engine) const {
	component_pool_ptr comp = type_.create_component(owner, *this, engine);
	// TODO Run assigners.
	return comp;
}

} // namespace entity
} // namespace mce
