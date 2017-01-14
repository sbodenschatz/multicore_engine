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

/// Data structure for representing a mesh in a static model including the geometry data.
struct static_model_mesh {
	std::string object_name;				 ///< The object name of the mesh.
	std::string group_name;					 ///< The group name of the mesh.
	std::vector<model::model_index> indices; ///< The index data for the triangles of the mesh.
};

/// Data structure for representing a static model including the geometry data.
struct static_model {
	std::vector<model::model_vertex> vertices; ///< The vertices used by the models triangles.
	std::vector<static_model_mesh> meshes;	 ///< The meshes of which the model consists.
};

} // namespace asset_gen
} // namespace mce

#endif /* ASSET_GEN_STATIC_MODEL_HPP_ */
