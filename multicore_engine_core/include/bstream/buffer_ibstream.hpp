/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/buffer_ibstream.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef BSTREAM_BUFFER_IBSTREAM_HPP_
#define BSTREAM_BUFFER_IBSTREAM_HPP_

#include <bstream/ibstream.hpp>
#include <cstdint>

namespace mce {
namespace bstream {

class buffer_ibstream : public ibstream {
	const char* buffer_;
	size_t size_;
	size_t read_position = 0;

public:
	buffer_ibstream(const char* buffer, size_t size) : buffer_{buffer}, size_{size} {};
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	virtual size_t size() const noexcept override;
	virtual size_t tell_read() const noexcept override;
	virtual void seek_read(size_t position) override;
	void reset() {
		clear_read_errors();
		read_position = 0;
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_BUFFER_IBSTREAM_HPP_ */
