/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/bstream/debug_print_obstream.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <bstream/debug_print_obstream.hpp>
#include <iomanip>

namespace mce {
namespace bstream {

debug_print_obstream::debug_print_obstream(std::ostream& debug_stream, bool hex)
		: debug_stream{debug_stream}, hex{hex} {
	if(hex) {
		debug_stream << std::hex << std::uppercase;
	} else {
		debug_stream << std::dec;
	}
}

bool debug_print_obstream::write_bytes(const char* buffer, size_t count) noexcept {
	debug_stream << "[ ";
	const unsigned char* ubuffer = reinterpret_cast<const unsigned char*>(buffer);
	int w = hex ? 2 : 3;
	for(size_t i = 0; i < count; ++i) {
		debug_stream << std::setw(w) << std::setfill('0') << int(ubuffer[i]) << " ";
	}
	debug_stream << "]\n";
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
