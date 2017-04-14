/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/util/offset_range.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_OFFSET_RANGE_HPP_
#define UTIL_OFFSET_RANGE_HPP_

#include <bstream/ibstream.hpp>
#include <bstream/obstream.hpp>
#include <exceptions.hpp>
#include <limits>
#include <type_traits>

namespace mce {
namespace util {

/// Implements a generic range of offsets as used for file formats and other things.
template <typename T>
class offset_range {
private:
	T begin_ = 0;
	T length_ = 0;

public:
	/// Creates an offset range from the given values.
	offset_range(T begin = 0, T length = 0) noexcept(std::is_nothrow_copy_constructible<T>::value)
			: begin_{begin}, length_{length} {}
	/// Returns the length of the offset range.
	T length() const noexcept(std::is_nothrow_copy_constructible<T>::value) {
		return length_;
	}
	/// Changes the length of the offset range.
	void length(T length) noexcept(std::is_nothrow_copy_assignable<T>::value) {
		length_ = length;
	}
	/// Returns the beginning of the offset range.
	T begin() const noexcept(std::is_nothrow_copy_constructible<T>::value) {
		return begin_;
	}
	/// Changes the beginning of the offset range.
	void begin(T begin) noexcept(std::is_nothrow_copy_assignable<T>::value) {
		begin_ = begin;
	}
	/// Calculates the end of the offset range from the beginning and length.
	T end() const noexcept(std::is_nothrow_copy_constructible<T>::value) {
		return begin_ + length_;
	}
	/// Converts the offset range to an offset range based on another data type.
	template <typename U>
	offset_range<U> convert_to() const {
		if(begin() > std::numeric_limits<U>::max())
			throw out_of_range_exception("Value of begin to big for target type.");
		if(length() > std::numeric_limits<U>::max())
			throw out_of_range_exception("Value of length to big for target type.");
		if(end() > std::numeric_limits<U>::max())
			throw out_of_range_exception("Value of end to big for target type.");
		return offset_range<U>(U(begin_), U(length_));
	}
	/// Allows writing offset ranges to binary streams.
	friend bstream::obstream& operator<<(bstream::obstream& stream, const offset_range<T>& value) {
		stream << value.begin_;
		stream << value.length_;
		return stream;
	}
	/// Allows reading offset ranges from binary streams.
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, offset_range<T>& value) {
		stream >> value.begin_;
		stream >> value.length_;
		return stream;
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_OFFSET_RANGE_HPP_ */
