/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/bstream.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef BSTREAM_IBSTREAM_HPP_
#define BSTREAM_IBSTREAM_HPP_

#include <cstdint>
#include <cstddef>

namespace mce {
namespace bstream {

class ibstream {
	bool eof_ = false;
	bool invalid_ = false;

protected:
	void raise_read_eof() noexcept {
		eof_ = true;
	}
	void raise_read_invalid() noexcept {
		invalid_ = true;
	}

public:
	virtual ~ibstream() = default;
	virtual size_t read_bytes(char* buffer, size_t count) noexcept = 0;
	virtual size_t size() const noexcept = 0;
	virtual size_t tell_read() const noexcept = 0;
	virtual void seek_read(size_t position) = 0;
	void clear_read_errors() noexcept {
		eof_ = false;
		invalid_ = false;
	}

	explicit operator bool() noexcept {
		return !errors();
	}

	bool errors() const noexcept {
		return eof() || invalid();
	}

	bool eof() const noexcept {
		return eof_;
	}

	bool invalid() const noexcept {
		return invalid_;
	}

	ibstream& operator>>(int8_t& value);
	ibstream& operator>>(int16_t& value);
	ibstream& operator>>(int32_t& value);
	ibstream& operator>>(int64_t& value);
	ibstream& operator>>(uint8_t& value);
	ibstream& operator>>(uint16_t& value);
	ibstream& operator>>(uint32_t& value);
	ibstream& operator>>(uint64_t& value);
	ibstream& operator>>(float& value);
	ibstream& operator>>(double& value);
	ibstream& operator>>(long double& value);
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_IBSTREAM_HPP_ */
