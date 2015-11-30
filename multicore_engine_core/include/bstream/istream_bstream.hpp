/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/istream_bstream.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef BSTREAM_ISTREAM_BSTREAM_HPP_
#define BSTREAM_ISTREAM_BSTREAM_HPP_

#include <bstream/ibstream.hpp>
#include <cstdint>
#include <memory>
#include <istream>

namespace mce {
namespace bstream {

class istream_bstream : public ibstream {
	std::istream& istream;

public:
	istream_bstream(std::istream& istream) : istream(istream){};
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	virtual size_t size() const noexcept override;
	virtual size_t tell_read() const noexcept override;
	virtual void seek_read(size_t position) override;
	void reset() {
		clear_read_errors();
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_ISTREAM_BSTREAM_HPP_ */
