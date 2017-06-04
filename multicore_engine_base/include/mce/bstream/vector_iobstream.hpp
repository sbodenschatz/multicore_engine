/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/bstream/vector_iobstream.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef BSTREAM_VECTOR_IOBSTREAM_HPP_
#define BSTREAM_VECTOR_IOBSTREAM_HPP_

/**
 * \file
 * Defines a binary stream working on top of a vector in memory.
 */

#include <mce/bstream/ibstream.hpp>
#include <mce/bstream/iobstream.hpp>
#include <mce/bstream/obstream.hpp>
#include <cstdint>
#include <vector>

namespace mce {
namespace bstream {

/// Implements a binary input / output stream backed by a dynamically growing vector of bytes.
class vector_iobstream : public iobstream {
	std::vector<char> data;
	size_t read_position = 0;
	size_t write_position = 0;

public:
	/// Creates a vector I/O binary stream.
	vector_iobstream(){};
	/// Reads bytes from the stream at the current read position.
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	/// Returns the current size of the vector buffer.
	virtual size_t size() const noexcept override;
	/// Returns the current read position.
	virtual size_t tell_read() const noexcept override;
	/// Changes the current read position.
	virtual void seek_read(size_t position) override;
	/// Writes bytes to the stream at the current write position possibly growing the vector buffer.
	virtual bool write_bytes(const char* buffer, size_t count) noexcept override;
	/// Returns the current write position.
	virtual size_t tell_write() const noexcept override;
	/// Changes the current write position.
	virtual void seek_write(size_t position) override;
	/// Clears all error flags.
	void reset() noexcept {
		clear_write_errors();
		clear_read_errors();
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_VECTOR_IOBSTREAM_HPP_ */
