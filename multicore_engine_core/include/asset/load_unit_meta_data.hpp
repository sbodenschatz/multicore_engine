/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/asset/load_unit_meta_data.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef ASSET_LOAD_UNIT_META_DATA_HPP_
#define ASSET_LOAD_UNIT_META_DATA_HPP_

#include <cstdint>
#include <vector>
#include <string>

namespace mce {
namespace bstream {
class ibstream;
class obstream;
} // namespace bstream
namespace asset {

struct asset_meta_data {
	uint64_t offset;
	uint64_t size;
	std::string name;
	friend bstream::ibstream& operator>>(bstream::ibstream& ibs, asset_meta_data& value);
	friend bstream::obstream& operator<<(bstream::obstream& obs, const asset_meta_data& value);
};

struct load_unit_meta_data {
	// std::string name;
	std::vector<asset_meta_data> assets;
	static const uint64_t magic_number = 0x4d43454c53422015ULL; /*MCELSB2015*/
	friend bstream::ibstream& operator>>(bstream::ibstream& ibs, load_unit_meta_data& value);
	friend bstream::obstream& operator<<(bstream::obstream& obs, const load_unit_meta_data& value);
};

} // namespace asset
} // namespace mce

#endif /* ASSET_LOAD_UNIT_META_DATA_HPP_ */
