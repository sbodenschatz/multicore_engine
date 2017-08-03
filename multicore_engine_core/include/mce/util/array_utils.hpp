/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/array_utils.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_ARRAY_UTILS_HPP_
#define MCE_UTIL_ARRAY_UTILS_HPP_

#include <array>

namespace mce {
namespace util {

template <typename... T>
std::array<std::common_type_t<T...>, sizeof...(T)> make_array(T&&... args) {
	return {std::forward<T>(args)...};
}

namespace detail {

template <typename T, size_t N, typename T_In, typename F, size_t... I>
std::array<T, N> array_transform_impl(T_In& input, F f, std::index_sequence<I...>) {
	return {f(std::get<I>(input))...};
}

} // namespace detail

template <typename T, typename T_In, typename F, size_t N = std::tuple_size<T_In>::value>
std::array<T, N> array_transform(T_In& input, F f) {
	return detail::array_transform_impl<T, N>(input, f, std::make_index_sequence<N>{});
}

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_ARRAY_UTILS_HPP_ */
