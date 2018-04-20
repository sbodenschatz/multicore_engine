/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/bstream/bstream.hpp
 * Copyright 2015-2018 by Stefan Bodenschatz
 */

#ifndef BSTREAM_IBSTREAM_HPP_
#define BSTREAM_IBSTREAM_HPP_

/**
 * \file
 * Defines the reading interface for binary streams.
 */

#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace mce {
namespace bstream {

/// Provides the abstract base class of input binary streams.
/**
 * It provides the interface that is used by other systems to read data types from binary data sources.
 * The specific data source used is implemented by the subclasses.
 */
class ibstream {
	bool eof_ = false;
	bool invalid_ = false;

public:
	/// Enables the end of file flag that indicates that no further data are available.
	void raise_read_eof() noexcept {
		eof_ = true;
	}
	/// Enables the invalid flag that indicates that invalid data were read.
	void raise_read_invalid() noexcept {
		invalid_ = true;
	}
	/// Provides a virtual destructor for subclasses to hook for destruction.
	virtual ~ibstream() = default;
	/// The hook for reading bytes from the implemented data source.
	virtual size_t read_bytes(char* buffer, size_t count) noexcept = 0;
	/// The hook for determining the size of the implemented data source.
	virtual size_t size() const noexcept = 0;
	/// The hook for determining the current read position in the implemented data source.
	virtual size_t tell_read() const noexcept = 0;
	/// The hook for changing the current read position in the implemented data source to the given value.
	virtual void seek_read(size_t position) = 0;

	/// Disables the end of file and invalid flags to reset previous error states.
	void clear_read_errors() noexcept {
		eof_ = false;
		invalid_ = false;
	}
	/// Checks if the stream is in a valid state (no error flags are enabled).
	explicit operator bool() noexcept {
		return !errors();
	}
	/// Checks if any error flag is enabled.
	bool errors() const noexcept {
		return eof() || invalid();
	}
	/// Checks if the end of file flag is set.
	bool eof() const noexcept {
		return eof_;
	}
	/// Checks if the invalid data flag is set.
	bool invalid() const noexcept {
		return invalid_;
	}

	/// Allows reading bool values from the stream.
	ibstream& operator>>(bool& value);
	/// Allows reading int8_t values from the stream.
	ibstream& operator>>(int8_t& value);
	/// Allows reading int16_t values from the stream.
	ibstream& operator>>(int16_t& value);
	/// Allows reading int32_t values from the stream.
	ibstream& operator>>(int32_t& value);
	/// Allows reading int64_t values from the stream.
	ibstream& operator>>(int64_t& value);
	/// Allows reading uint8_t values from the stream.
	ibstream& operator>>(uint8_t& value);
	/// Allows reading uint16_t values from the stream.
	ibstream& operator>>(uint16_t& value);
	/// Allows reading uint32_t values from the stream.
	ibstream& operator>>(uint32_t& value);
	/// Allows reading uint64_t values from the stream.
	ibstream& operator>>(uint64_t& value);
	/// Allows reading float values from the stream.
	ibstream& operator>>(float& value);
	/// Allows reading double values from the stream.
	ibstream& operator>>(double& value);
	/// Allows reading long double values from the stream.
	ibstream& operator>>(long double& value);
	/// Allows reading vectors of values from the stream (unsigned 64-bit length followed by the elements).
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
	/// Allows reading of text strings from the stream (unsigned 64-bit length followed by 8-bit chars).
	ibstream& operator>>(std::string& value);
	/// Allows reading fixed size arrays of other supported types from the stream.
	template <typename T, size_t N>
	ibstream& operator>>(T (&value)[N]) {
		for(size_t i = 0; i < N && *this; ++i) {
			*this >> value[i];
		}
		return *this;
	}
	/// Allows reading of 1D glm vectors from the stream.
	template <typename T, glm::qualifier P>
	ibstream& operator>>(glm::tvec1<T, P>& value) {
		*this >> value.x;
		return *this;
	}
	/// Allows reading of 2D glm vectors from the stream.
	template <typename T, glm::qualifier P>
	ibstream& operator>>(glm::tvec2<T, P>& value) {
		*this >> value.x;
		*this >> value.y;
		return *this;
	}
	/// Allows reading of 3D glm vectors from the stream.
	template <typename T, glm::qualifier P>
	ibstream& operator>>(glm::tvec3<T, P>& value) {
		*this >> value.x;
		*this >> value.y;
		*this >> value.z;
		return *this;
	}
	/// Allows reading of 4D glm vectors from the stream.
	template <typename T, glm::qualifier P>
	ibstream& operator>>(glm::tvec4<T, P>& value) {
		*this >> value.x;
		*this >> value.y;
		*this >> value.z;
		*this >> value.w;
		return *this;
	}
	/// Allows reading of glm quaternions from the stream.
	template <typename T, glm::qualifier P>
	ibstream& operator>>(glm::tquat<T, P>& value) {
		*this >> value.x;
		*this >> value.y;
		*this >> value.z;
		*this >> value.w;
		return *this;
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_IBSTREAM_HPP_ */
