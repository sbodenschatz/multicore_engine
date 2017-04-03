/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/bstream/debug_print_obstream.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef BSTREAM_DEBUG_PRINT_OBSTREAM_HPP_
#define BSTREAM_DEBUG_PRINT_OBSTREAM_HPP_

#include "obstream.hpp"
#include <ostream>

namespace mce {
namespace bstream {

/// Provides a debug implementation of a binary output stream that logs to a text representation.
class debug_print_obstream : public obstream {
	std::ostream& debug_stream;
	size_t write_position = 0;
	size_t written_size = 0;
	bool hex;
	bool print_bytes = true;
	bool print_integer_signed = false;
	bool print_integer_unsigned = false;
	bool print_float = false;
	bool print_double = false;
	bool print_long_double = false;
	bool print_char = false;

	void print_bytes_impl(const char* buffer, size_t count) noexcept;
	void print_integer_signed_impl(const char* buffer, size_t count) noexcept;
	void print_integer_unsigned_impl(const char* buffer, size_t count) noexcept;
	void print_float_impl(const char* buffer, size_t count) noexcept;
	void print_double_impl(const char* buffer, size_t count) noexcept;
	void print_long_double_impl(const char* buffer, size_t count) noexcept;
	void print_char_impl(const char* buffer, size_t count) noexcept;

public:
	/// Creates a debug binary stream that logs to the given stream using hex or decimal mode a specified.
	debug_print_obstream(std::ostream& debug_stream, bool hex = true);
	/// Hooks into the write process to log the written data.
	virtual bool write_bytes(const char* buffer, size_t count) noexcept override;
	/// Returns the size of the virtually written file.
	virtual size_t size() const noexcept override;
	/// Returns the current write position within the virtual output file.
	virtual size_t tell_write() const noexcept override;
	/// Changes the current write position within the virtual output file.
	virtual void seek_write(size_t position) override;

	/// Log the written data as the values of bytes.
	void enable_print_bytes() noexcept {
		print_bytes = true;
	}
	/// Don't log the written data as the values of bytes.
	void disable_print_bytes() noexcept {
		print_bytes = false;
	}
	/// Log the written data as the values of signed integers.
	void enable_print_integer_signed() noexcept {
		print_integer_signed = true;
	}
	/// Don't log the written data as the values of signed integers.
	void disable_print_integer_signed() noexcept {
		print_integer_signed = false;
	}
	/// Log the written data as the values of unsigned integers.
	void enable_print_integer_unsigned() noexcept {
		print_integer_unsigned = true;
	}
	/// Don't log the written data as the values of unsigned integers.
	void disable_print_integer_unsigned() noexcept {
		print_integer_unsigned = false;
	}
	/// Log the written data as the values of floats.
	void enable_print_float() noexcept {
		print_float = true;
	}
	/// Don't log the written data as the values of floats.
	void disable_print_float() noexcept {
		print_float = false;
	}
	/// Log the written data as the values of doubles.
	void enable_print_double() noexcept {
		print_double = true;
	}
	/// Don't log the written data as the values of doubles.
	void disable_print_double() noexcept {
		print_double = false;
	}
	/// Log the written data as the values of long doubles.
	void enable_print_long_double() noexcept {
		print_long_double = true;
	}
	/// Don't log the written data as the values of long doubles.
	void disable_print_long_double() noexcept {
		print_long_double = false;
	}
	/// Log the written data as characters.
	void enable_print_char() noexcept {
		print_char = true;
	}
	/// Don't log the written data as characters.
	void disable_print_char() noexcept {
		print_char = false;
	}
};

} /* namespace bstream */
} /* namespace mce */

#endif /* BSTREAM_DEBUG_PRINT_OBSTREAM_HPP_ */
