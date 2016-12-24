/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset/vertex_struct.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_STATIC_MODEL_HPP_
#define ASSET_GEN_STATIC_MODEL_HPP_

#include <cstdint>
#include <glm/glm.hpp>
#include <model/model_format.hpp>
#include <util/offset_range.hpp>

namespace mce {
namespace asset_gen {

struct static_model_mesh {
	std::string object_name;
	std::string group_name;
	std::vector<model::model_index> indices;
};

struct static_model {
	std::vector<model::model_vertex> vertices;
	std::vector<static_model_mesh> meshes;
};

} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_STATIC_MODEL_HPP_ */