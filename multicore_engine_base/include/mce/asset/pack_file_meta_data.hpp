/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/asset/pack_file_meta_data.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef ASSET_PACK_FILE_META_DATA_HPP_
#define ASSET_PACK_FILE_META_DATA_HPP_

/**
 * \file
 * Defines the serializable structure of the meta data of a pack file.
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

/// Represents the meta data for an element of a pack file.
struct pack_file_element_meta_data {
	uint64_t offset;		  ///< The offset of the element within the pack file.
	uint64_t size;			  ///< The size of the element.
	uint64_t compressed_size; ///< The compressed size of the element (0 means uncompressed).
	std::string name;		  ///< The name of the element.

	/// Deserializes the pack file element meta data from the bstream.
	friend bstream::ibstream& operator>>(bstream::ibstream& ibs, pack_file_element_meta_data& value);
	/// Serializes the pack file meta element data from the bstream.
	friend bstream::obstream& operator<<(bstream::obstream& obs, const pack_file_element_meta_data& value);
};

/// Represents the meta data for a pack file.
struct pack_file_meta_data {
	/// The supported (current) version of the pack file meta data format.
	static constexpr uint8_t current_version[3] = {0, 2, 0};

	/// Magic number for pack file meta data files.
	static constexpr uint64_t magic_number = 0x4d43455053422015ULL; /*MCEPSB2015*/
	std::vector<pack_file_element_meta_data> elements;				///< The elements of the pack file.
	/// Version flag for the pack file meta data format.
	uint8_t version[3] = {current_version[0], current_version[1], current_version[2]};

	/// Deserializes the pack file meta data from the bstream.
	friend bstream::ibstream& operator>>(bstream::ibstream& ibs, pack_file_meta_data& value);
	/// Serializes the load unit meta data from the bstream.
	friend bstream::obstream& operator<<(bstream::obstream& obs, const pack_file_meta_data& value);
};

/// Deserializes the pack file element meta data from the bstream.
bstream::ibstream& operator>>(bstream::ibstream& ibs, pack_file_element_meta_data& value);
/// Serializes the pack file meta element data from the bstream.
bstream::obstream& operator<<(bstream::obstream& obs, const pack_file_element_meta_data& value);
/// Deserializes the pack file meta data from the bstream.
bstream::ibstream& operator>>(bstream::ibstream& ibs, pack_file_meta_data& value);
/// Serializes the load unit meta data from the bstream.
bstream::obstream& operator<<(bstream::obstream& obs, const pack_file_meta_data& value);

} // namespace asset
} // namespace mce

#endif /* ASSET_PACK_FILE_META_DATA_HPP_ */
