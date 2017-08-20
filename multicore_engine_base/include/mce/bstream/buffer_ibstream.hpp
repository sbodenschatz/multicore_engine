/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/bstream/buffer_ibstream.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef BSTREAM_BUFFER_IBSTREAM_HPP_
#define BSTREAM_BUFFER_IBSTREAM_HPP_

/**
 * \file
 * Defines binary streams reading from buffers.
 */

#include <cstdint>
#include <mce/bstream/ibstream.hpp>
#include <memory>

namespace mce {
namespace bstream {

/// Provides a binary stream interface for reading data from an arbitrary buffer in memory.
class buffer_ibstream : public ibstream {
	const char* buffer_;
	size_t size_;
	size_t read_position = 0;
	std::shared_ptr<const char> owned_buffer;

public:
	/// Constructs a binary stream interface for a buffer in memory without taking ownership.
	buffer_ibstream(const char* buffer, size_t size) : buffer_{buffer}, size_{size} {};
	/// Constructs a binary stream interface for a buffer manager by a shared_ptr.
	buffer_ibstream(const std::shared_ptr<const char>& owned_buffer, size_t size)
			: buffer_{owned_buffer.get()}, size_{size}, owned_buffer{owned_buffer} {};
	/// Reads bytes from the buffer at the current read position.
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	/// Returns the size of the buffer as specified during stream creation.
	virtual size_t size() const noexcept override;
	/// Returns the current read position in the buffer.
	virtual size_t tell_read() const noexcept override;
	/// Changes the current read position in the buffer.
	virtual void seek_read(size_t position) override;
	/// Resets the state of the stream by clearing error flags and resetting the read position to 0.
	void reset() noexcept {
		clear_read_errors();
		read_position = 0;
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_BUFFER_IBSTREAM_HPP_ */
