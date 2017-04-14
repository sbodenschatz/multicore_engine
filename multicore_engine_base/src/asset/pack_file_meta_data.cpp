/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/pack_file_meta_data.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <asset/pack_file_meta_data.hpp>
#include <bstream/ibstream.hpp>
#include <bstream/obstream.hpp>
#include <algorithm>
#include <iterator>

namespace mce {
namespace asset {

constexpr uint64_t pack_file_meta_data::magic_number;
constexpr uint8_t pack_file_meta_data::current_version[3];

bstream::ibstream& operator>>(bstream::ibstream& ibs, pack_file_element_meta_data& value) {
	ibs >> value.offset;
	ibs >> value.size;
	ibs >> value.compressed_size;
	ibs >> value.name;
	return ibs;
}
bstream::obstream& operator<<(bstream::obstream& obs, const pack_file_element_meta_data& value) {
	obs << value.offset;
	obs << value.size;
	obs << value.compressed_size;
	obs << value.name;
	return obs;
}

bstream::ibstream& operator>>(bstream::ibstream& ibs, pack_file_meta_data& value) {
	uint64_t magic_num = 0;
	ibs >> magic_num;
	if(magic_num != pack_file_meta_data::magic_number) {
		ibs.raise_read_invalid();
	} else {
		ibs >> value.version;

		if(!std::equal(std::begin(value.version), std::end(value.version),
					   std::begin(pack_file_meta_data::current_version),
					   std::end(pack_file_meta_data::current_version))) {
			ibs.raise_read_invalid();
		} else {
			ibs >> value.elements;
		}
	}
	return ibs;
}
bstream::obstream& operator<<(bstream::obstream& obs, const pack_file_meta_data& value) {
	obs << pack_file_meta_data::magic_number;
	obs << value.version;
	obs << value.elements;
	return obs;
}

} // namespace asset
} // namespace mce
