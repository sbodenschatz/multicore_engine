/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/pack_file_meta_data.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_PACK_FILE_META_DATA_HPP_
#define ASSET_PACK_FILE_META_DATA_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace mce {
namespace bstream {
class ibstream;
class obstream;
} // namespace bstream
namespace asset {

struct pack_file_element_meta_data {
	uint64_t offset;
	uint64_t size;
	std::string name;
	friend bstream::ibstream& operator>>(bstream::ibstream& ibs, pack_file_element_meta_data& value);
	friend bstream::obstream& operator<<(bstream::obstream& obs, const pack_file_element_meta_data& value);
};

struct pack_file_meta_data {
	std::vector<pack_file_element_meta_data> elements;
	static const uint64_t magic_number = 0x4d43455053422015ULL; /*MCEPSB2015*/
	friend bstream::ibstream& operator>>(bstream::ibstream& ibs, pack_file_meta_data& value);
	friend bstream::obstream& operator<<(bstream::obstream& obs, const pack_file_meta_data& value);
};

} // namespace asset
} // namespace mce

#endif /* ASSET_PACK_FILE_META_DATA_HPP_ */
