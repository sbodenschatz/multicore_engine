/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/util/offset_range.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_OFFSET_RANGE_HPP_
#define UTIL_OFFSET_RANGE_HPP_

#include <bstream/ibstream.hpp>
#include <bstream/obstream.hpp>
#include <exceptions.hpp>
#include <limits>
#include <stdexcept>

namespace mce {
namespace util {

template <typename T>
class offset_range {
private:
	T begin_ = 0;
	T length_ = 0;

public:
	offset_range(T begin = 0, T length = 0) : begin_{begin}, length_{length} {}

	T length() const {
		return length_;
	}

	void length(T length) {
		length_ = length;
	}

	T begin() const {
		return begin_;
	}

	void begin(T begin) {
		begin_ = begin;
	}

	T end() const {
		return begin_ + length_;
	}

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
	friend bstream::obstream& operator<<(bstream::obstream& stream, const offset_range<T>& value) {
		stream << value.begin_;
		stream << value.length_;
		return stream;
	}
	friend bstream::ibstream& operator>>(bstream::ibstream& stream, offset_range<T>& value) {
		stream >> value.begin_;
		stream >> value.length_;
		return stream;
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_OFFSET_RANGE_HPP_ */
