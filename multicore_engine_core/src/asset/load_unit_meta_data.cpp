/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/load_unit_meta_data.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/load_unit_meta_data.hpp>
#include <bstream/ibstream.hpp>
#include <bstream/obstream.hpp>

namespace mce {
namespace asset {

const uint64_t load_unit_meta_data::magic_number;

bstream::ibstream& operator>>(bstream::ibstream& ibs, asset_meta_data& value) {
	ibs >> value.offset;
	ibs >> value.size;
	ibs >> value.name;
	return ibs;
}
bstream::obstream& operator<<(bstream::obstream& obs, const asset_meta_data& value) {
	obs << value.offset;
	obs << value.size;
	obs << value.name;
	return obs;
}

bstream::ibstream& operator>>(bstream::ibstream& ibs, load_unit_meta_data& value) {
	uint64_t magic_num = 0;
	ibs >> magic_num;
	if(magic_num != load_unit_meta_data::magic_number)
		ibs.raise_read_invalid();
	else
		ibs >> value.assets;
	return ibs;
}
bstream::obstream& operator<<(bstream::obstream& obs, const load_unit_meta_data& value) {
	obs << load_unit_meta_data::magic_number;
	obs << value.assets;
	return obs;
}

} // namespace asset
} // namespace mce
