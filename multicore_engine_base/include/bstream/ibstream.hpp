/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/bstream.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef BSTREAM_IBSTREAM_HPP_
#define BSTREAM_IBSTREAM_HPP_

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace mce {
namespace bstream {

class ibstream {
	bool eof_ = false;
	bool invalid_ = false;

public:
	void raise_read_eof() noexcept {
		eof_ = true;
	}
	void raise_read_invalid() noexcept {
		invalid_ = true;
	}

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
	template <typename T>
	ibstream& operator>>(std::vector<T>& value) {
		uint64_t size = 0;
		value.clear();
		(*this) >> size;
		if(size > value.max_size()) {
			raise_read_eof();
		} else {
			value.reserve(size_t(size));
			T entry;
			for(uint64_t i = 0; i < size; ++i) {
				(*this) >> entry;
				value.push_back(entry);
			}
		}
		return *this;
	}
	ibstream& operator>>(std::string& value);
	template <typename T, size_t N>
	ibstream& operator>>(T (&value)[N]) {
		for(size_t i = 0; i < N && *this; ++i) {
			*this >> value[i];
		}
		return *this;
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_IBSTREAM_HPP_ */
