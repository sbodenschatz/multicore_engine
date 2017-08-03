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
	return {{std::forward<T>(args)...}};
}

namespace detail {

template <typename T, size_t N, typename T_In, typename F, size_t... I>
std::array<T, N> array_transform_impl(T_In& input, F f, std::index_sequence<I...>) {
	return {{f(std::get<I>(input))...}};
}

template <typename T, size_t N, typename T_In1, typename T_In2, typename F, size_t... I>
std::array<T, N> array_transform_impl(T_In1& input1, T_In2& input2, F f, std::index_sequence<I...>) {
	return {{f(std::get<I>(input1), std::get<I>(input2))...}};
}

template <typename T, size_t N, typename F, size_t... I>
std::array<T, N> array_generate_impl(F f, std::index_sequence<I...>) {
	return {{(static_cast<void>(I), f())...}};
}

template <typename T, size_t N, typename F, size_t... I>
std::array<T, N> array_generate_indexed_impl(F f, std::index_sequence<I...>) {
	return {{f(I)...}};
}

} // namespace detail

template <typename T, typename T_In, typename F, size_t N = std::tuple_size<T_In>::value>
std::array<T, N> array_transform(T_In& input, F f) {
	return detail::array_transform_impl<T, N>(input, f, std::make_index_sequence<N>{});
}

template <typename T, typename T_In1, typename T_In2, typename F,
		  size_t N = std::min(std::tuple_size<T_In1>::value, std::tuple_size<T_In2>::value)>
std::array<T, N> array_transform(T_In1& input1, T_In2& input2, F f) {
	return detail::array_transform_impl<T, N>(input1, input2, f, std::make_index_sequence<N>{});
}

template <typename T, size_t N, typename F>
std::array<T, N> array_generate_indexed(F f) {
	return detail::array_generate_indexed_impl<T, N>(f, std::make_index_sequence<N>{});
}

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_ARRAY_UTILS_HPP_ */
