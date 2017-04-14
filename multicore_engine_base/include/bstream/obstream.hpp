/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/bstream/obstream.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef BSTREAM_OBSTREAM_HPP_
#define BSTREAM_OBSTREAM_HPP_

#include <glm/glm.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace mce {
namespace bstream {

/// Provides the abstract base class of output binary streams.
/**
 * It provides the interface that is used by other systems to write data types to binary data targets.
 * The specific data target used is implemented by the subclasses.
 */
class obstream {
	bool eof_ = false;
	bool invalid_ = false;

public:
	/// Enables the end of file flag that indicates that the target is full.
	void raise_write_eof() noexcept {
		eof_ = true;
	}
	/// Enables the invalid flag that indicates that invalid data were written.
	void raise_write_invalid() noexcept {
		invalid_ = true;
	}

	/// Provides a virtual destructor for subclasses to hook for destruction.
	virtual ~obstream() = default;
	/// The hook for writing bytes to the implemented data target.
	virtual bool write_bytes(const char* buffer, size_t count) noexcept = 0;
	/// The hook for determining the size of the implemented data target.
	virtual size_t size() const noexcept = 0;
	/// The hook for determining the current write position in the implemented data target.
	virtual size_t tell_write() const noexcept = 0;
	/// The hook for changing the current write position in the implemented data target to the given value.
	virtual void seek_write(size_t position) = 0;

	/// Disables the end of file and invalid flags to reset previous error states.
	void clear_write_errors() noexcept {
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

	/// Allows writing int8_t values to the stream.
	obstream& operator<<(const int8_t& value);
	/// Allows writing int16_t values to the stream.
	obstream& operator<<(const int16_t& value);
	/// Allows writing int32_t values to the stream.
	obstream& operator<<(const int32_t& value);
	/// Allows writing int64_t values to the stream.
	obstream& operator<<(const int64_t& value);
	/// Allows writing uint8_t values to the stream.
	obstream& operator<<(const uint8_t& value);
	/// Allows writing uint16_t values to the stream.
	obstream& operator<<(const uint16_t& value);
	/// Allows writing uint32_t values to the stream.
	obstream& operator<<(const uint32_t& value);
	/// Allows writing uint64_t values to the stream.
	obstream& operator<<(const uint64_t& value);
	/// Allows writing float values to the stream.
	obstream& operator<<(const float& value);
	/// Allows writing double values to the stream.
	obstream& operator<<(const double& value);
	/// Allows writing long double values to the stream.
	obstream& operator<<(const long double& value);
	/// Allows writing vectors of values to the stream (unsigned 64-bit length followed by the elements).
	template <typename T>
	obstream& operator<<(const std::vector<T>& value) {
		uint64_t size = value.size();
		(*this) << size;
		for(auto& entry : value) {
			(*this) << entry;
		}
		return *this;
	}
	/// Allows writing of text strings to the stream (unsigned 64-bit length followed by 8-bit chars).
	obstream& operator<<(const std::string& value);
	/// Allows writing fixed size arrays of other supported types to the stream.
	template <typename T, size_t N>
	obstream& operator<<(const T(&value)[N]) {
		for(size_t i = 0; i < N && *this; ++i) {
			*this << value[i];
		}
		return *this;
	}
	/// Allows writing of 1D glm vectors to the stream.
	template <typename T, glm::precision P>
	obstream& operator<<(const glm::tvec1<T, P>& value) {
		*this << value.x;
		return *this;
	}
	/// Allows writing of 2D glm vectors to the stream.
	template <typename T, glm::precision P>
	obstream& operator<<(const glm::tvec2<T, P>& value) {
		*this << value.x;
		*this << value.y;
		return *this;
	}
	/// Allows writing of 3D glm vectors to the stream.
	template <typename T, glm::precision P>
	obstream& operator<<(const glm::tvec3<T, P>& value) {
		*this << value.x;
		*this << value.y;
		*this << value.z;
		return *this;
	}
	/// Allows writing of 4D glm vectors to the stream.
	template <typename T, glm::precision P>
	obstream& operator<<(const glm::tvec4<T, P>& value) {
		*this << value.x;
		*this << value.y;
		*this << value.z;
		*this << value.w;
		return *this;
	}
	/// Allows writing of glm quaternions to the stream.
	template <typename T, glm::precision P>
	obstream& operator<<(const glm::tquat<T, P>& value) {
		*this << value.x;
		*this << value.y;
		*this << value.z;
		*this << value.w;
		return *this;
	}
};

} // namespace bstream
} // namespace mce

#endif /* BSTREAM_OBSTREAM_HPP_ */
