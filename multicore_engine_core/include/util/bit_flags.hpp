/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/bit_flags.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef UTIL_BIT_FLAGS_HPP_
#define UTIL_BIT_FLAGS_HPP_

#include <boost/operators.hpp>
#include <type_traits>

namespace mce {
namespace util {

namespace detail {

template <typename enum_t, unsigned int mask_size = 0u>
struct bit_flags_negate_helper {
	static constexpr std::underlying_type_t<enum_t> all_bits() {
		std::underlying_type_t<enum_t> ret = 0;
		for(unsigned int i = 0; i < mask_size; ++i) {
			ret |= 1 << i;
		}
		return ret;
	}
};

template <typename enum_t>
struct bit_flags_negate_helper<enum_t, 0u> {};

} // namespace detail

template <typename enum_t, unsigned int mask_size = 0>
class bit_flags : boost::bitwise<bit_flags<enum_t, mask_size>,
								 boost::equality_comparable<bit_flags<enum_t, mask_size>>> {
public:
	typedef std::underlying_type_t<enum_t> value_t;
	bit_flags() : value{0} {}
	// cppcheck-suppress noExplicitConstructor
	bit_flags(enum_t bit) : value{static_cast<value_t>(bit)} {}
	// cppcheck-suppress passedByValue
	explicit bit_flags(value_t value) : value{value} {}

	bool operator==(const bit_flags<enum_t, mask_size>& other) const {
		return other.value == value;
	}
	bit_flags<enum_t, mask_size>& operator&=(const bit_flags<enum_t, mask_size>& other) {
		value &= other.value;
		return *this;
	}
	bit_flags<enum_t, mask_size>& operator|=(const bit_flags<enum_t, mask_size>& other) {
		value |= other.value;
		return *this;
	}
	bit_flags<enum_t, mask_size>& operator^=(const bit_flags<enum_t, mask_size>& other) {
		value ^= other.value;
		return *this;
	}
	bool operator!() const {
		return !value;
	}
	explicit operator bool() const {
		return !operator!();
	}
	explicit operator value_t() const {
		return value;
	}

	template <unsigned int mask_size_ = mask_size, typename = std::enable_if<(mask_size_ > 0)>>
	bit_flags<enum_t, mask_size> operator~() const {
		auto temp = *this;
		temp.value ^= detail::bit_flags_negate_helper<enum_t, mask_size_>::all_bits();
		return temp;
	}

private:
	value_t value;
};

} // namespace util
} // namespace mce

#endif /* UTIL_BIT_FLAGS_HPP_ */
