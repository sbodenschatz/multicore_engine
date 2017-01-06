/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/vector_iobstream.hpp
 * Copyright 2015-2016 by Stefan Bodenschatz
 */

#ifndef BSTREAM_VECTOR_IOBSTREAM_HPP_
#define BSTREAM_VECTOR_IOBSTREAM_HPP_

#include <bstream/iobstream.hpp>
#include <cstdint>
#include <vector>

namespace mce {
namespace bstream {

class vector_iobstream : public iobstream {
	std::vector<char> data;
	size_t read_position = 0;
	size_t write_position = 0;

public:
	vector_iobstream(){};
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	virtual size_t size() const noexcept override;
	virtual size_t tell_read() const noexcept override;
	virtual void seek_read(size_t position) override;
	virtual bool write_bytes(const char* buffer, size_t count) noexcept override;
	virtual size_t tell_write() const noexcept override;
	virtual void seek_write(size_t position) override;
	void reset() noexcept {
		clear_write_errors();
		clear_read_errors();
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_VECTOR_IOBSTREAM_HPP_ */
