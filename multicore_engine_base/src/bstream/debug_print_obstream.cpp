/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/bstream/debug_print_obstream.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <bstream/debug_print_obstream.hpp>

namespace mce {
namespace bstream {

bool debug_print_obstream::write_bytes(const char* buffer, size_t count) noexcept {
	debug_stream << "[ ";
	for(size_t i = 0; i < count; ++i) {
		debug_stream << int(buffer[i]) << " ";
	}
	debug_stream << "]";
	write_position += count;
	written_size = std::max(written_size, write_position);
	return true;
}
size_t debug_print_obstream::size() const noexcept {
	return written_size;
}
size_t debug_print_obstream::tell_write() const noexcept {
	return write_position;
}
void debug_print_obstream::seek_write(size_t position) {
	write_position = std::min(written_size, position);
}

} /* namespace bstream */
} /* namespace mce */
