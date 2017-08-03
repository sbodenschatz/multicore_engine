/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/array_utils.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_ARRAY_UTILS_HPP_
#define MCE_UTIL_ARRAY_UTILS_HPP_

/**
 * \file
 * Defines utility functions for working with std::array.
 */

#include <array>

namespace mce {
namespace util {

/// \brief Creates a std::array from the given values using the std::common_type to determine the type of the
/// array elements.
/**
 * Represents a simplified implementation of std::(experimental)::make_array (in Lib Fundamentals v2 which is
 * not generally available yet). The simplification is that this implementation does not support specifying
 * the array element type manually yet.
 */
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

/// Returns a std::array containing the results of applying the function object f to each element of input.
/**
 * The result element type T must be specified manually.
 * The type of input (T_In) must support std::get<Index>() and std::tuple_size<T_In>.
 * The function object f must be callable with the result of std::get<Index>() for each element in input as a
 * parameter and return a value that can be used for the array initialization.
 *
 * The order of function calls for the elements is not specified because the initialization list evaluation
 * order is unspecified.
 */
template <typename T, typename T_In, typename F, size_t N = std::tuple_size<T_In>::value>
std::array<T, N> array_transform(T_In& input, F f) {
	return detail::array_transform_impl<T, N>(input, f, std::make_index_sequence<N>{});
}

/// \brief Returns a std::array containing the results of applying the function object f to each pair of
/// corresponding elements of the both inputs.
/**
 * The result element type T must be specified manually.
 * The types of input1 and input2 (T_In1 and T_In2) must support std::get<Index>() and std::tuple_size<T_In1>
 * or std::tuple_size<T_In2>.
 * The function object f must be callable with f(std::get<Index>(input1),std::get<Index>(input2)) for each
 * index from 0 (including) to min(std::tuple_size<T_In1>::value, std::tuple_size<T_In2>::value) (excluding)
 * and return a value that can be used for the array initialization.
 *
 * The order of function calls for the elements is not specified because the initialization list evaluation
 * order is unspecified.
 */
template <typename T, typename T_In1, typename T_In2, typename F,
		  size_t N = std::min(std::tuple_size<T_In1>::value, std::tuple_size<T_In2>::value)>
std::array<T, N> array_transform(T_In1& input1, T_In2& input2, F f) {
	return detail::array_transform_impl<T, N>(input1, input2, f, std::make_index_sequence<N>{});
}

/// Returns a std::array containing the result of calling f for each index in the array.
/**
 * The element type T and the number of elements must be specified manually.
 * The function object f must be callable with a size_t index as the sole parameter and return a value that
 * can be used for the array initialization.
 *
 * The order of function calls for the indices is not specified because the initialization list evaluation
 * order is unspecified.
 */
template <typename T, size_t N, typename F>
std::array<T, N> array_generate_indexed(F f) {
	return detail::array_generate_indexed_impl<T, N>(f, std::make_index_sequence<N>{});
}

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_ARRAY_UTILS_HPP_ */
