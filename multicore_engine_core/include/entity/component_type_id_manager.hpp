/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/component_type_id_manager.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_COMPONENT_TYPE_ID_MANAGER_HPP_
#define ENTITY_COMPONENT_TYPE_ID_MANAGER_HPP_

#include "ecs_types.hpp"

namespace mce {
namespace entity {

/// Provides a way to assign unique ids to component types.
/**
 * The ids generated are dependent on the order in which the id member function is first called for a given
 * type.
 */
class component_type_id_manager {
	static component_type_id_t next_id;

public:
	/// Returns a unique id for the given type T.
	template <typename T>
	static component_type_id_t id() {
		static component_type_id_t id_ = next_id++;
		return id_;
	}
};

} // namespace entity
} // namespace mce

#endif /* ENTITY_COMPONENT_TYPE_ID_MANAGER_HPP_ */
