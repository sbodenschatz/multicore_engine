/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/bit_flags.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef UTIL_BIT_FLAGS_HPP_
#define UTIL_BIT_FLAGS_HPP_

#include <boost/operators.hpp>
#include <type_traits>

namespace mce {
namespace util {

namespace detail {

template <typename Bits_Enum, unsigned int mask_size = 0u>
struct bit_flags_negate_helper {
	static constexpr std::underlying_type_t<Bits_Enum> all_bits() {
		std::underlying_type_t<Bits_Enum> ret = 0;
		for(unsigned int i = 0; i < mask_size; ++i) {
			ret |= 1 << i;
		}
		return ret;
	}
};

template <typename Bits_Enum>
struct bit_flags_negate_helper<Bits_Enum, 0u> {};

} // namespace detail

/// Provides a type-safe implementation of flag combinations in the form of bit flags.
/**
 * The available bits are expected to be provided in Bits_Enum in the form of an enum class.
 * If negation of a bit_flags value is required, the bits are expected to be neighboring bits in the least
 * significant bit positions and the number of bits must be supplied in mask_size.
 */
template <typename Bits_Enum, unsigned int mask_size = 0>
class bit_flags : boost::bitwise<bit_flags<Bits_Enum, mask_size>,
								 boost::equality_comparable<bit_flags<Bits_Enum, mask_size>>> {
public:
	/// The underlying (integer) type used to store the value.
	typedef std::underlying_type_t<Bits_Enum> value_t;
	/// Creates an empty bit_flags with no bits set.
	bit_flags() : value{0} {}
	/// Creates a bit_flags with only the given bit set.
	// cppcheck-suppress noExplicitConstructor
	bit_flags(Bits_Enum bit) : value{static_cast<value_t>(bit)} {}
	/// Creates a bit_flags from the given non-type-safe bit flag value.
	// cppcheck-suppress passedByValue
	explicit bit_flags(value_t value) : value{value} {}

	/// Compares *this and other for equality.
	/**
	 * The != operator is provided by boost::equality_comparable.
	 */
	bool operator==(const bit_flags<Bits_Enum, mask_size>& other) const {
		return other.value == value;
	}
	/// Unsets all flags in *this that are not in other.
	/**
	 * The operator & to filter flags into a copy is provided by boost::bitwise.
	 */
	bit_flags<Bits_Enum, mask_size>& operator&=(const bit_flags<Bits_Enum, mask_size>& other) {
		value &= other.value;
		return *this;
	}
	/// Sets all flags in *this that are in other additionally to the ones already set in *this.
	/**
	 * The operator | to combine flags into a copy is provided by boost::bitwise.
	 */
	bit_flags<Bits_Enum, mask_size>& operator|=(const bit_flags<Bits_Enum, mask_size>& other) {
		value |= other.value;
		return *this;
	}
	/// Flips all flags in *this that are also in other.
	/**
	 * The operator ^ to get the difference of flags into a copy is provided by boost::bitwise.
	 */
	bit_flags<Bits_Enum, mask_size>& operator^=(const bit_flags<Bits_Enum, mask_size>& other) {
		value ^= other.value;
		return *this;
	}
	/// Checks if no flag is set in *this.
	bool operator!() const {
		return !value;
	}
	/// Checks if any flag is set in *this.
	explicit operator bool() const {
		return !operator!();
	}
	/// Converts *this into the underlying non-type-safe representation.
	explicit operator value_t() const {
		return value;
	}
	/// Returns a copy of the bit_flags that has all bits flipped.
	/**
	 * Only available if a mask_size was provided as a template parameter for the bit_flags and requires the
	 * bits in Bits_Enum to be packed into the least significant bit positions.
	 */
	template <unsigned int mask_size_ = mask_size, typename = std::enable_if<(mask_size_ > 0)>>
	bit_flags<Bits_Enum, mask_size> operator~() const {
		auto temp = *this;
		temp.value ^= detail::bit_flags_negate_helper<Bits_Enum, mask_size_>::all_bits();
		return temp;
	}

private:
	value_t value;
};

} // namespace util
} // namespace mce

#endif /* UTIL_BIT_FLAGS_HPP_ */
