/*
 * Multi-Core Engine project
 * File /multicore_engine_base/include/util/composite_magic_number.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_COMPOSITE_MAGIC_NUMBER_HPP_
#define UTIL_COMPOSITE_MAGIC_NUMBER_HPP_

namespace mce {
namespace util {

template <typename T, typename A>
constexpr T composite_magic_number(A arg) {
	return T(arg);
}

template <typename T, typename A, typename... Args>
constexpr T composite_magic_number(A arg, Args... args) {
	return composite_magic_number<T>(args...) << (sizeof(arg) * 8) | T(arg);
}

} // namespace util
} // namespace mce

#endif /* UTIL_COMPOSITE_MAGIC_NUMBER_HPP_ */
