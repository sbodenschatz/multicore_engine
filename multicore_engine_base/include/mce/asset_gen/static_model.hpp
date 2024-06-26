/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/asset/vertex_struct.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_GEN_STATIC_MODEL_HPP_
#define ASSET_GEN_STATIC_MODEL_HPP_

/**
 * \file
 * Defines data structures to represent processed models in memory in a tooling context (not in the engine).
 */

#include <mce/model/model_format.hpp>
#include <string>
#include <vector>

namespace mce {
namespace asset_gen {

/// Data structure for representing a mesh in a static model including the geometry data.
struct static_model_mesh {
	std::string object_name;				 ///< The object name of the mesh.
	std::string group_name;					 ///< The group name of the mesh.
	std::string material_name;				 ///< The name of the material assigned to the mesh.
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
