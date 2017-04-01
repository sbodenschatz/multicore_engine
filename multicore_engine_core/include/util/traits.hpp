/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/traits.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_TRAITS_HPP_
#define UTIL_TRAITS_HPP_

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

} // namespace util
} // namespace mce

#endif /* UTIL_TRAITS_HPP_ */
