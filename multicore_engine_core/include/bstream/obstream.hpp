/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/obstream.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef BSTREAM_OBSTREAM_HPP_
#define BSTREAM_OBSTREAM_HPP_

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

namespace mce {
namespace bstream {

class obstream {
	bool eof_ = false;
	bool invalid_ = false;

public:
	void raise_write_eof() noexcept {
		eof_ = true;
	}
	void raise_write_invalid() noexcept {
		invalid_ = true;
	}

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
	template <typename T>
	obstream& operator<<(const std::vector<T>& value) {
		uint64_t size = value.size();
		(*this) << size;
		for(auto& entry : value) {
			(*this) << entry;
		}
		return *this;
	}
	obstream& operator<<(const std::string& value);
	template <typename T, size_t N>
	obstream& operator<<(const T(&value)[N]) {
		for(size_t i = 0; i < N && *this; ++i) {
			*this << value[i];
		}
		return *this;
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_OBSTREAM_HPP_ */
