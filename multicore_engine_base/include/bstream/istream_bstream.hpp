/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/istream_bstream.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef BSTREAM_ISTREAM_BSTREAM_HPP_
#define BSTREAM_ISTREAM_BSTREAM_HPP_

#include <bstream/ibstream.hpp>
#include <cstdint>
#include <istream>
#include <memory>

namespace mce {
namespace bstream {

/// Implements an adapter that provides a binary stream interface on top of standard input streams.
class istream_bstream : public ibstream {
	std::istream& istream;

public:
	/// Creates an adapter for the given input stream but takes no ownership of the original stream.
	// cppcheck-suppress noExplicitConstructor
	istream_bstream(std::istream& istream) : istream(istream){};
	/// Reads bytes from the underlying stream.
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	/// Returns the size of the underlying stream by using seekg to end and tellg.
	virtual size_t size() const noexcept override;
	/// Returns the current read position of the underlying stream.
	virtual size_t tell_read() const noexcept override;
	/// Changes the current read position of the underlying stream.
	virtual void seek_read(size_t position) override;
	/// Clears all error flags.
	void reset() noexcept {
		clear_read_errors();
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_ISTREAM_BSTREAM_HPP_ */
