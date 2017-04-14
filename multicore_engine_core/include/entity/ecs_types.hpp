/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/ecs_types.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_ECS_TYPES_HPP_
#define ENTITY_ECS_TYPES_HPP_

#include <containers/smart_pool_ptr.hpp>
#include <entity/component.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>

namespace mce {
namespace entity {

class component;

/// Specifies the type used for the ids of entities.
typedef uint64_t entity_id_t;
/// Specifies the type used for component type ids.
typedef uint32_t component_type_id_t;
/// Specifies the type used to store the position of an entity.
typedef glm::vec4 entity_position_t;
/// Specifies the type used to store the orientation of an entity.
typedef glm::quat entity_orientation_t;
/// Specifies the smart pointer type used to manage the lifetime of component objects.
typedef mce::containers::smart_pool_ptr<mce::entity::component> component_pool_ptr;

} // namespace entity
} // namespace mce

#endif /* ENTITY_ECS_TYPES_HPP_ */
