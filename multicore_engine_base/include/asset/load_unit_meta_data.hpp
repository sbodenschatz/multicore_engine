/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/load_unit_meta_data.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_LOAD_UNIT_META_DATA_HPP_
#define ASSET_LOAD_UNIT_META_DATA_HPP_

/**
 * \file
 * Defines the serializable structure of the meta data of a load unit.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace mce {
namespace bstream {
class ibstream;
class obstream;
} // namespace bstream
namespace asset {

/// Defines the structure of the meta data for an asset within a load unit.
struct asset_meta_data {
	uint64_t offset;  ///< The offset of the asset into the containing load unit payload in bytes.
	uint64_t size;	///< The size of the assets in bytes.
	std::string name; ///< The internal name of the asset.

	/// Deserializes the asset meta data from the bstream.
	friend bstream::ibstream& operator>>(bstream::ibstream& ibs, asset_meta_data& value);
	/// Serializes the asset meta data to the bstream.
	friend bstream::obstream& operator<<(bstream::obstream& obs, const asset_meta_data& value);
};

/// Defines the structure of the meta data for a load unit.
struct load_unit_meta_data {
	// std::string name;
	std::vector<asset_meta_data> assets; ///< The meta data for the assets in the load unit.

	/// Magic number for load unit meta data files.
	static const uint64_t magic_number = 0x4d43454c53422015ULL; /*MCELSB2015*/

	/// Deserializes the load unit meta data from the bstream.
	friend bstream::ibstream& operator>>(bstream::ibstream& ibs, load_unit_meta_data& value);
	/// Serializes the load unit meta data to the bstream.
	friend bstream::obstream& operator<<(bstream::obstream& obs, const load_unit_meta_data& value);
};

/// Deserializes the asset meta data from the bstream.
bstream::ibstream& operator>>(bstream::ibstream& ibs, asset_meta_data& value);
/// Serializes the asset meta data to the bstream.
bstream::obstream& operator<<(bstream::obstream& obs, const asset_meta_data& value);
/// Deserializes the load unit meta data from the bstream.
bstream::ibstream& operator>>(bstream::ibstream& ibs, load_unit_meta_data& value);
/// Serializes the load unit meta data to the bstream.
bstream::obstream& operator<<(bstream::obstream& obs, const load_unit_meta_data& value);

} // namespace asset
} // namespace mce

#endif /* ASSET_LOAD_UNIT_META_DATA_HPP_ */
