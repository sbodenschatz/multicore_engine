/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/native_file_bstream.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef BSTREAM_IOSTREAM_BSTREAM_HPP_
#define BSTREAM_IOSTREAM_BSTREAM_HPP_

#include <bstream/iobstream.hpp>
#include <cstdint>
#include <memory>
#include <istream>

namespace mce {
namespace asset {
class asset;
} // namespace asset
namespace bstream {

class iostream_bstream : public iobstream {
	std::iostream& iostream;

public:
	iostream_bstream(std::iostream& iostream) : iostream(iostream){};
	virtual size_t read_bytes(char* buffer, size_t count) noexcept override;
	virtual size_t size() const noexcept override;
	virtual size_t tell_read() const noexcept override;
	virtual void seek_read(size_t position) override;
	virtual bool write_bytes(const char* buffer, size_t count) noexcept override;
	virtual size_t tell_write() const noexcept override;
	virtual void seek_write(size_t position) override;
	void reset() {
		clear_write_errors();
		clear_read_errors();
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_IOSTREAM_BSTREAM_HPP_ */
