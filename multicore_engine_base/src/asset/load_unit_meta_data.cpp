/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/asset/load_unit_meta_data.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <mce/asset/load_unit_meta_data.hpp>
#include <mce/bstream/ibstream.hpp>
#include <mce/bstream/obstream.hpp>
#include <mce/exceptions.hpp>

namespace mce {
namespace asset {

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
	ibs >> value.magic_number;
	if(value.magic_number != load_unit_meta_data::magic_number_) {
		ibs.raise_read_invalid();
		throw invalid_magic_number_exception("Invalid magic number.");
	}
	ibs >> value.version;
	if(value.version != load_unit_meta_data::version_) {
		ibs.raise_read_invalid();
		throw invalid_version_exception("Can't load different load unit version.");
	}
	ibs >> value.assets;
	return ibs;
}
bstream::obstream& operator<<(bstream::obstream& obs, const load_unit_meta_data& value) {
	obs << value.magic_number;
	obs << value.version;
	obs << value.assets;
	return obs;
}

} // namespace asset
} // namespace mce
