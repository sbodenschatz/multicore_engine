/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/bstream/buffer_ibstream.cpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#include <mce/bstream/buffer_ibstream.hpp>
#include <cstdint>
#include <mce/exceptions.hpp>
#include <algorithm>
#include <cstring>

namespace mce {
namespace bstream {

size_t buffer_ibstream::read_bytes(char* buffer, size_t count) noexcept {
	if(read_position >= size_) return 0u;
	size_t size = std::min(size_ - read_position, count);
	std::memcpy(buffer, buffer_ + read_position, size);
	read_position += size;
	return size;
}

size_t buffer_ibstream::size() const noexcept {
	return size_;
}
size_t buffer_ibstream::tell_read() const noexcept {
	return read_position;
}
void buffer_ibstream::seek_read(size_t position) {
	if(position <= size_) {
		read_position = position;
	} else {
		throw out_of_range_exception("Position outside of the buffer.");
	}
}

} // namespace bstream
} // namespace mce
