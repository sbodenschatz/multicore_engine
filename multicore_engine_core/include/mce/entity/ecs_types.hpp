/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/entity/ecs_types.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ENTITY_ECS_TYPES_HPP_
#define ENTITY_ECS_TYPES_HPP_

/**
 * \file
 * Provides basic typedefs for the entity component system.
 */

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <mce/containers/smart_pool_ptr.hpp>
#include <mce/entity/component.hpp>

namespace mce {
namespace containers {
template <typename T>
class simple_smart_object_pool;
} // namespace containers
namespace entity {

class component;

/// Specifies the type used for the ids of entities.
typedef uint64_t entity_id_t;
/// Specifies the type used for component type ids.
typedef uint32_t component_type_id_t;
/// Specifies the type used to store the position of an entity.
typedef glm::vec3 entity_position_t;
/// Specifies the type used to store the orientation of an entity.
typedef glm::quat entity_orientation_t;

#ifdef MCE_USE_BLOCKED_COMPONENT_POOLS
/// Specifies the smart pointer type used to manage the lifetime of component objects.
typedef mce::containers::smart_pool_ptr<mce::entity::component> component_pool_ptr;
/// brief Specifies the smart pointer template used to manage the lifetime of component objects referring to
/// the implementation type.
template <typename T>
using component_impl_pool_ptr = mce::containers::smart_pool_ptr<T>;
/// Specifies the template for systems and system states to use to store component objects.
template <typename T, size_t block_size = 0x10000u>
using component_pool = mce::containers::smart_object_pool<T, block_size>;
#else
/// Specifies the smart pointer type used to manage the lifetime of component objects.
typedef std::shared_ptr<mce::entity::component> component_pool_ptr;
/// brief Specifies the smart pointer template used to manage the lifetime of component objects referring to
/// the implementation type.
template <typename T>
using component_impl_pool_ptr = std::shared_ptr<T>;
/// Specifies the template for systems and system states to use to store component objects.
template <typename T, size_t = 0x10000u>
using component_pool = mce::containers::simple_smart_object_pool<T>;
#endif

} // namespace entity
} // namespace mce

#endif /* ENTITY_ECS_TYPES_HPP_ */
