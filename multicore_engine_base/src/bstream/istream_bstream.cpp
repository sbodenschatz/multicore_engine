/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/bstream/istream_bstream.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <mce/bstream/istream_bstream.hpp>
#include <cstdint>

namespace mce {
namespace bstream {

size_t istream_bstream::read_bytes(char* buffer, size_t count) noexcept {
	istream.read(buffer, count);
	return istream.gcount();
}
size_t istream_bstream::size() const noexcept {
	auto old_pos = istream.tellg();
	istream.seekg(0, std::ios::end);
	auto size = istream.tellg();
	istream.seekg(old_pos, std::ios::beg);
	return size;
}
size_t istream_bstream::tell_read() const noexcept {
	return istream.tellg();
}
void istream_bstream::seek_read(size_t position) {
	istream.seekg(position, std::ios::beg);
}

} // namespace bstream
} // namespace mce
