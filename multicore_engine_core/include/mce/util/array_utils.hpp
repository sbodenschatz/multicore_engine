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

namespace detail {

template <typename T, typename...>
struct make_array_element_type {
	using type = T;
};

template <typename... Args>
struct make_array_element_type<void, Args...> {
	using type = std::common_type_t<Args...>;
};

template <typename T, size_t N, size_t... I>
std::array<std::remove_cv_t<T>, N> to_array_impl(T (&raw_array)[N], std::index_sequence<I...>) {
	return {{raw_array[I]...}};
}

template <typename T, size_t N, size_t... I>
std::array<std::remove_cv_t<T>, N> to_array_impl(T(&&raw_array)[N], std::index_sequence<I...>) {
	return {{std::move(raw_array[I])...}};
}

} // namespace detail

/// \brief Creates a std::array from the given values using either a given type T or the std::common_type of
/// parameters to determine the type of the array elements.
/**
 * Represents a simplified implementation of std::(experimental)::make_array (in Lib Fundamentals v2 which is
 * not generally available yet).
 * The simplification is that this implementation does not check for an attempt to use a reference_wrapper
 * when the type is chosen automatically, which is not supported.
 */
template <typename T = void, typename... Args>
std::array<typename detail::make_array_element_type<T, Args...>::type, sizeof...(Args)>
make_array(Args&&... args) {
	return {{std::forward<Args>(args)...}};
}

/// Creates a std::array from the given built-in array (taken as a L-value).
/**
 * Represents an implementation of std::(experimental)::to_array (in Lib Fundamentals v2 which is not
 * generally available yet).
 */
template <typename T, size_t N>
std::array<std::remove_cv_t<T>, N> to_array(T (&raw_array)[N]) {
	return detail::to_array_impl(raw_array, std::make_index_sequence<N>{});
}

/// Creates a std::array from the given built-in array (taken as a R-value).
/**
 * Works similar to the L-value version but takes R-values and moves the elements into the std::array.
 */
template <typename T, size_t N>
std::array<std::remove_cv_t<T>, N> to_array(T(&&raw_array)[N]) {
	return detail::to_array_impl(std::move(raw_array), std::make_index_sequence<N>{});
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
