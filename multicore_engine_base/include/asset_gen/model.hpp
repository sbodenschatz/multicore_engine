/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset/vertex_struct.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_MODEL_HPP_
#define ASSET_GEN_MODEL_HPP_

#include <cstdint>
#include <glm/glm.hpp>

namespace mce {
namespace asset_gen {

struct model_vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

using model_index = uint32_t;

class model {};

class model_collision_data {};

} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_MODEL_HPP_ */
