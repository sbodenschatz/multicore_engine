/*
 * Multi-Core Engine project
 * File /multicore_engine_core/src/bstream/vector_iobstream.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <cstdint>
#include <mce/bstream/vector_iobstream.hpp>

namespace mce {
namespace bstream {

size_t vector_iobstream::read_bytes(char* buffer, size_t count) noexcept {
	size_t i = 0;
	for(; i < count && read_position < data.size(); ++i, ++read_position) {
		buffer[i] = data[read_position];
	}
	return i;
}
size_t vector_iobstream::size() const noexcept {
	return data.size();
}
size_t vector_iobstream::tell_read() const noexcept {
	return read_position;
}
void vector_iobstream::seek_read(size_t position) {
	read_position = position;
}
bool vector_iobstream::write_bytes(const char* buffer, size_t count) noexcept {
	size_t i = 0;
	size_t old_write_position = write_position;
	for(; i < count && write_position < data.size(); ++i, ++write_position) {
		data[write_position] = buffer[i];
	}
	try {
		for(; i < count; ++i, ++write_position) {
			data.push_back(buffer[i]);
		}
		return true;
	} catch(...) {
		write_position = old_write_position;
		return false;
	}
}
size_t vector_iobstream::tell_write() const noexcept {
	return write_position;
}
void vector_iobstream::seek_write(size_t position) {
	write_position = position;
}

} // namespace bstream
} // namespace mce
