/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/traits.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_TRAITS_HPP_
#define UTIL_TRAITS_HPP_

/**
 * \file
 * Provides custom type traits.
 */

#include <utility>

namespace mce {
namespace util {
namespace detail {
using std::swap;

template <typename T>
struct swappable_trait_impl {
	struct not_swappable_t;

	template <typename U = T, typename V = decltype(swap(std::declval<U&>(), std::declval<U&>()))>
	static V test_func(U&, U&);
	template <typename... Args>
	static not_swappable_t test_func(Args&...);

	static constexpr bool value =
			!std::is_same<not_swappable_t,
						  decltype(test_func(std::declval<T&>(), std::declval<T&>()))>::value;
};

template <typename T, bool>
struct nothrow_swappable_trait_impl {
	static constexpr bool value = noexcept(swap(std::declval<T&>(), std::declval<T&>()));
};

template <typename T>
struct nothrow_swappable_trait_impl<T, false> {
	static constexpr bool value = false;
};

} // namespace detail

/// Replacement for std::is_swappable<T> from C++17 where it is not available.
/**
 * \bug This may give false positives where swap is not SFINAEd correctly (for C++17) yet. Unfortunately this
 * can not be prevented without also replacing std::swap and excluding the original from ADL.
 */
template <typename T>
struct is_swappable {
	/// Contains true if T is swappable or false if T is not swappable.
	static constexpr bool value = detail::swappable_trait_impl<T>::value;
};

template <typename T>
constexpr bool is_swappable<T>::value;

/// Replacement for std::is_nothrow_swappable<T> from C++17 where it is not available.
/**
 * \bug This may give false positives where swap is not SFINAEd correctly (for C++17) yet. Unfortunately this
 * can not be prevented without also replacing std::swap and excluding the original from ADL.
 */
template <typename T>
struct is_nothrow_swappable {
	/// Contains true if T is nothrow-swappable or false if T is not nothrow-swappable.
	static constexpr bool value =
			detail::nothrow_swappable_trait_impl<T, detail::swappable_trait_impl<T>::value>::value;
};

template <typename T>
constexpr bool is_nothrow_swappable<T>::value;

/// Type function to convert a type to the parameter / return value type for setters and getters.
template <typename T>
struct accessor_value_type {
	/// Is <code>T</code> if <code>T</code> is a fundamental type or <code>const T&</code> otherwise.
	using type = typename std::conditional<std::is_fundamental<T>::value, T, const T&>::type;
};

/// Provides a shorthand for <code>typename accessor_value_type<T>::type</code>.
template <typename T>
using accessor_value_type_t = typename accessor_value_type<T>::type;

namespace detail {

template <typename T, typename... Args>
struct callable_trait_impl {
	struct not_callable_t;
	template <typename U = T, typename V = decltype(std::declval<U>()(std::declval<Args>()...))>
	static V test_func(const Args&...);
	template <typename... Args2>
	static not_callable_t test_func(const Args2&...);

	static constexpr bool value =
			!std::is_same<not_callable_t, decltype(test_func(std::declval<Args>()...))>::value;
};

} // namespace detail

template <typename T, typename... Args>
struct is_callable {
	static constexpr bool value = detail::callable_trait_impl<T, Args...>::value;
};


} // namespace util
} // namespace mce

#endif /* UTIL_TRAITS_HPP_ */
