/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/bstream/debug_print_obstream.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef BSTREAM_DEBUG_PRINT_OBSTREAM_HPP_
#define BSTREAM_DEBUG_PRINT_OBSTREAM_HPP_

#include "obstream.hpp"
#include <ostream>

namespace mce {
namespace bstream {

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
	debug_print_obstream(std::ostream& debug_stream, bool hex = true);
	~debug_print_obstream() = default;
	virtual bool write_bytes(const char* buffer, size_t count) noexcept override;
	virtual size_t size() const noexcept override;
	virtual size_t tell_write() const noexcept override;
	virtual void seek_write(size_t position) override;

	void enable_print_bytes() noexcept {
		print_bytes = true;
	}
	void disable_print_bytes() noexcept {
		print_bytes = false;
	}
	void enable_print_integer_signed() noexcept {
		print_integer_signed = true;
	}
	void disable_print_integer_signed() noexcept {
		print_integer_signed = false;
	}
	void enable_print_integer_unsigned() noexcept {
		print_integer_unsigned = true;
	}
	void disable_print_integer_unsigned() noexcept {
		print_integer_unsigned = false;
	}
	void enable_print_float() noexcept {
		print_float = true;
	}
	void disable_print_float() noexcept {
		print_float = false;
	}
	void enable_print_double() noexcept {
		print_double = true;
	}
	void disable_print_double() noexcept {
		print_double = false;
	}
	void enable_print_long_double() noexcept {
		print_long_double = true;
	}
	void disable_print_long_double() noexcept {
		print_long_double = false;
	}
	void enable_print_char() noexcept {
		print_char = true;
	}
	void disable_print_char() noexcept {
		print_char = false;
	}
};

} /* namespace bstream */
} /* namespace mce */

#endif /* BSTREAM_DEBUG_PRINT_OBSTREAM_HPP_ */
