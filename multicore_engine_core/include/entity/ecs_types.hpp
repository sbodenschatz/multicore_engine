/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/entity/ecs_types.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ENTITY_ECS_TYPES_HPP_
#define ENTITY_ECS_TYPES_HPP_

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <containers/smart_pool_ptr.hpp>

namespace mce {
namespace entity {

class component;

typedef uint64_t entity_id_t;
typedef uint32_t component_type_id_t;
typedef glm::vec4 entity_position_t;
typedef glm::quat entity_orientation_t;
typedef mce::containers::smart_pool_ptr<mce::entity::component> component_pool_ptr;

} // namespace entity
} // namespace mce

#endif /* ENTITY_ECS_TYPES_HPP_ */
