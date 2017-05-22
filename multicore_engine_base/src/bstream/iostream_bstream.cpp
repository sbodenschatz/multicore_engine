/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/bstream/iostream_bstream.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <mce/bstream/iostream_bstream.hpp>
#include <cstdint>

namespace mce {
namespace bstream {

size_t iostream_bstream::read_bytes(char* buffer, size_t count) noexcept {
	iostream.read(buffer, count);
	return iostream.gcount();
}
size_t iostream_bstream::size() const noexcept {
	auto old_pos = iostream.tellg();
	iostream.seekg(0, std::ios::end);
	auto size = iostream.tellg();
	iostream.seekg(old_pos, std::ios::beg);
	return size;
}
size_t iostream_bstream::tell_read() const noexcept {
	return iostream.tellg();
}
void iostream_bstream::seek_read(size_t position) {
	iostream.seekg(position, std::ios::beg);
}
bool iostream_bstream::write_bytes(const char* buffer, size_t count) noexcept {
	iostream.write(buffer, count);
	return bool(iostream);
}
size_t iostream_bstream::tell_write() const noexcept {
	return iostream.tellp();
}
void iostream_bstream::seek_write(size_t position) {
	iostream.seekp(position, std::ios::beg);
}

} // namespace bstream
} // namespace mce
