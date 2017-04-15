/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/native_file_bstream.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef BSTREAM_IOSTREAM_BSTREAM_HPP_
#define BSTREAM_IOSTREAM_BSTREAM_HPP_

#include <bstream/ibstream.hpp>
#include <bstream/iobstream.hpp>
#include <bstream/obstream.hpp>
#include <cstdint>
#include <istream>

namespace mce {
namespace bstream {

/// Implements an adapter that provides a binary stream interface on top of standard I/O streams.
class iostream_bstream : public iobstream {
	std::iostream& iostream;

public:
	/// Creates an adapter for the given I/O stream but takes no ownership of the original stream.
	// cppcheck-suppress noExplicitConstructor
	iostream_bstream(std::iostream& iostream) : iostream(iostream){};
	/// Reads bytes from the underlying stream.
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	/// Returns the size of the underlying stream by using seekg to end and tellg.
	virtual size_t size() const noexcept override;
	/// Returns the current read position of the underlying stream.
	virtual size_t tell_read() const noexcept override;
	/// Changes the current read position of the underlying stream.
	virtual void seek_read(size_t position) override;
	/// Writes bytes to the underlying stream.
	virtual bool write_bytes(const char* buffer, size_t count) noexcept override;
	/// Returns the current write position of the underlying stream.
	virtual size_t tell_write() const noexcept override;
	/// Changes the current write position of the underlying stream.
	virtual void seek_write(size_t position) override;
	/// Clears all error flags.
	void reset() noexcept {
		clear_write_errors();
		clear_read_errors();
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_IOSTREAM_BSTREAM_HPP_ */
