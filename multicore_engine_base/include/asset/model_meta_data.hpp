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

struct static_mesh_meta_data {
	std::string name;
	offset_range index_data;
};

struct static_model_meta_data {
	std::string name;
	offset_range vertex_data;
	std::vector<static_mesh_meta_data> meshes;
};

} // namespace asset
} // namespace mce

#endif /* ASSET_MODEL_META_DATA_HPP_ */
