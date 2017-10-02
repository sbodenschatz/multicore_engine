/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/pack_file_meta_data.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <iterator>
#include <mce/asset/pack_file_meta_data.hpp>
#include <mce/bstream/ibstream.hpp>
#include <mce/bstream/obstream.hpp>
#include <mce/exceptions.hpp>

namespace mce {
namespace asset {

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
	ibs >> value.magic_number;
	if(value.magic_number != pack_file_meta_data::magic_number_) {
		ibs.raise_read_invalid();
		throw invalid_magic_number_exception("Invalid magic number.");
	}
	ibs >> value.version;
	if(value.version != pack_file_meta_data::version_) {
		ibs.raise_read_invalid();
		throw invalid_version_exception("Can't load different load unit version.");
	}
	ibs >> value.elements;
	return ibs;
}
bstream::obstream& operator<<(bstream::obstream& obs, const pack_file_meta_data& value) {
	obs << value.magic_number;
	obs << value.version;
	obs << value.elements;
	return obs;
}

} // namespace asset
} // namespace mce
