/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/reflection/type.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <bstream/ibstream.hpp>
#include <bstream/obstream.hpp>
#include <reflection/type.hpp>
#include <type_traits>

namespace mce {
namespace reflection {

std::string type_parser<std::vector<std::string>>::delimiter = ";";

bstream::ibstream& operator>>(bstream::ibstream& ibs, type_t& value) {
	return ibs >> reinterpret_cast<std::underlying_type_t<type_t>&>(value);
}
bstream::obstream& operator<<(bstream::obstream& obs, type_t value) {
	return obs << static_cast<std::underlying_type_t<type_t>>(value);
}

} // namespace reflection
} // namespace mce
