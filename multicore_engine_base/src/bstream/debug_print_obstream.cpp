/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/bstream/debug_print_obstream.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <mce/bstream/debug_print_obstream.hpp>
#include <cstddef>
#include <mce/memory/aligned.hpp>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>

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

void debug_print_obstream::print_bytes_impl(const char* buffer, size_t count) noexcept {
	debug_stream << "[ ";
	const unsigned char* ubuffer = reinterpret_cast<const unsigned char*>(buffer);
	int w = hex ? 2 : 3;
	for(size_t i = 0; i < count; ++i) {
		debug_stream << std::setw(w) << std::setfill('0') << int(ubuffer[i]) << " ";
	}
	debug_stream << "]";
}

template <typename T>
static bool try_print_integer_signed_impl(std::ostream& debug_stream, const char* buffer, size_t count,
										  const char* suffix) noexcept {
	if(!memory::is_aligned(buffer, alignof(T))) return false;
	if(count % sizeof(T)) return false;
	const T* tbuffer = reinterpret_cast<const T*>(buffer);
	size_t tcount = count / sizeof(T);
	debug_stream << "< ";
	for(size_t i = 0; i < tcount; ++i) {
		int64_t tmp = int64_t(tbuffer[i]);
		if((tmp & 0x7FFFFFFFFFFFFFFF) == 0) { // prevent overflow
			debug_stream << "-8000000000000000" << suffix << " ";
		} else if(tmp < 0) {
			debug_stream << "-" << -tmp << suffix << " ";
		} else {
			debug_stream << tmp << suffix << " ";
		}
	}
	debug_stream << ">";
	return true;
}

template <typename T>
static bool try_print_integer_unsigned_impl(std::ostream& debug_stream, const char* buffer, size_t count,
											const char* suffix) noexcept {
	if(!memory::is_aligned(buffer, alignof(T))) return false;
	if(count % sizeof(T)) return false;
	const T* tbuffer = reinterpret_cast<const T*>(buffer);
	size_t tcount = count / sizeof(T);
	debug_stream << "< ";
	for(size_t i = 0; i < tcount; ++i) {
		debug_stream << uint64_t(tbuffer[i]) << suffix << " ";
	}
	debug_stream << ">";
	return true;
}

template <typename T>
static bool try_print_floating_point_impl(std::ostream& debug_stream, const char* buffer, size_t count,
										  const char* suffix) {
	if(!memory::is_aligned(buffer, alignof(T))) return false;
	if(count % sizeof(T)) return false;
	const T* tbuffer = reinterpret_cast<const T*>(buffer);
	size_t tcount = count / sizeof(T);
	debug_stream << "{ ";
	for(size_t i = 0; i < tcount; ++i) {
		debug_stream << tbuffer[i] << suffix << " ";
	}
	debug_stream << "}";
	return true;
}

void debug_print_obstream::print_integer_signed_impl(const char* buffer, size_t count) noexcept {
	bool ok = try_print_integer_signed_impl<int64_t>(debug_stream, buffer, count, "_s64");
	ok = try_print_integer_signed_impl<int32_t>(debug_stream, buffer, count, "_s32") || ok;
	ok = try_print_integer_signed_impl<int16_t>(debug_stream, buffer, count, "_s16") || ok;
	ok = try_print_integer_signed_impl<int8_t>(debug_stream, buffer, count, "_s8") || ok;
	if(!ok) {
		debug_stream << "<>";
	}
}
void debug_print_obstream::print_integer_unsigned_impl(const char* buffer, size_t count) noexcept {
	bool ok = try_print_integer_unsigned_impl<uint64_t>(debug_stream, buffer, count, "_u64");
	ok = try_print_integer_unsigned_impl<uint32_t>(debug_stream, buffer, count, "_u32") || ok;
	ok = try_print_integer_unsigned_impl<uint16_t>(debug_stream, buffer, count, "_u16") || ok;
	ok = try_print_integer_unsigned_impl<uint8_t>(debug_stream, buffer, count, "_u8") || ok;
	if(!ok) {
		debug_stream << "<>";
	}
}

void debug_print_obstream::print_float_impl(const char* buffer, size_t count) noexcept {
	if(!try_print_floating_point_impl<float>(debug_stream, buffer, count, "f")) {
		debug_stream << "{f}";
	}
}
void debug_print_obstream::print_double_impl(const char* buffer, size_t count) noexcept {
	if(!try_print_floating_point_impl<double>(debug_stream, buffer, count, "d")) {
		debug_stream << "{d}";
	}
}
void debug_print_obstream::print_long_double_impl(const char* buffer, size_t count) noexcept {
	if(!try_print_floating_point_impl<long double>(debug_stream, buffer, count, "ld")) {
		debug_stream << "{ld}";
	}
}
void debug_print_obstream::print_char_impl(const char* buffer, size_t count) noexcept {
	debug_stream << "( ";
	for(size_t i = 0; i < count; ++i) {
		if(buffer[i] < ' ') {
			debug_stream << int(buffer[i]) << " ";
		} else {
			debug_stream << "'" << buffer[i] << "' ";
		}
	}
	debug_stream << ")";
}

bool debug_print_obstream::write_bytes(const char* buffer, size_t count) noexcept {
	if(print_bytes) print_bytes_impl(buffer, count);
	if(print_integer_signed) print_integer_signed_impl(buffer, count);
	if(print_integer_unsigned) print_integer_unsigned_impl(buffer, count);
	if(print_float) print_float_impl(buffer, count);
	if(print_double) print_double_impl(buffer, count);
	if(print_long_double) print_long_double_impl(buffer, count);
	if(print_char) print_char_impl(buffer, count);
	debug_stream << "\n";
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
	debug_stream << write_position << ":\n";
}

} /* namespace bstream */
} /* namespace mce */
