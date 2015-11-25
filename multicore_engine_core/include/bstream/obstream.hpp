/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/obstream.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef BSTREAM_OBSTREAM_HPP_
#define BSTREAM_OBSTREAM_HPP_

#include <cstdint>
#include <cstddef>

namespace mce {
namespace bstream {

class obstream {
	bool eof_ = false;
	bool invalid_ = false;

protected:
	void raise_write_eof() noexcept {
		eof_ = true;
	}
	void raise_write_invalid() noexcept {
		invalid_ = true;
	}

public:
	virtual ~obstream() = default;
	virtual bool write_bytes(const char* buffer, size_t count) noexcept = 0;
	virtual size_t size() const noexcept = 0;
	virtual size_t tell_write() const noexcept = 0;
	virtual void seek_write(size_t position) = 0;
	void clear_write_errors() noexcept {
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

	obstream& operator<<(const int8_t& value);
	obstream& operator<<(const int16_t& value);
	obstream& operator<<(const int32_t& value);
	obstream& operator<<(const int64_t& value);
	obstream& operator<<(const uint8_t& value);
	obstream& operator<<(const uint16_t& value);
	obstream& operator<<(const uint32_t& value);
	obstream& operator<<(const uint64_t& value);
	obstream& operator<<(const float& value);
	obstream& operator<<(const double& value);
	obstream& operator<<(const long double& value);
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_OBSTREAM_HPP_ */
