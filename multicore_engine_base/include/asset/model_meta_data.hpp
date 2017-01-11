/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/asset/model_meta_data.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef ASSET_MODEL_META_DATA_HPP_
#define ASSET_MODEL_META_DATA_HPP_

#include <asset/asset_defs.hpp>
#include <vector>

namespace mce {
namespace asset {

/// Represents the meta data for a single static mesh.
struct static_mesh_meta_data {
	std::string name;		 ///< The name of the static mesh.
	offset_range index_data; ///< The offset range of the index data of the static mesh within the model file.
};

/// Represents the meta data for a static model consisting of arbitrarily many static meshes.
struct static_model_meta_data {
	std::string name; ///< The name of the model.

	/// The offset range of the vertex data of the static model within the model file.
	offset_range vertex_data;
	std::vector<static_mesh_meta_data> meshes; ///< The meta data for the meshes in the model.
};

} // namespace asset
} // namespace mce

#endif /* ASSET_MODEL_META_DATA_HPP_ */
