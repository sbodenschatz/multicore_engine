/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/mce/util/composite_magic_number.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_COMPOSITE_MAGIC_NUMBER_HPP_
#define UTIL_COMPOSITE_MAGIC_NUMBER_HPP_

/**
 * \file
 * Defines a tool function to compose magic numbers (for file formats and protocols) from smaller constants
 * (e.g. letters).
 */

namespace mce {
namespace util {
namespace detail {

template <typename T, typename A>
constexpr T composite_magic_number(A arg) noexcept {
	return T(arg);
}

template <typename T, typename A, typename... Args>
constexpr T composite_magic_number(A arg, Args... args) noexcept {
	return composite_magic_number<T>(args...) << (sizeof(arg) * 8) | T(arg);
}

} // namespace detail

/// Creates a magic number from smaller constants.
template <typename T, typename... Args>
constexpr T composite_magic_number(Args... args) noexcept {
	return detail::composite_magic_number<T>(args...);
}

} // namespace util
} // namespace mce

#endif /* UTIL_COMPOSITE_MAGIC_NUMBER_HPP_ */
