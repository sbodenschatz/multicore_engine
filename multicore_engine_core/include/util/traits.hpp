/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/traits.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_TRAITS_HPP_
#define UTIL_TRAITS_HPP_

namespace mce {
namespace util {
namespace detail {
using std::swap;

template <typename T>
struct swappable_trait_impl {
	struct not_swappable_t;

	template <typename U = T>
	static auto test_func(U& a, U& b) -> decltype(swap(a, b));
	template <typename... Args>
	static not_swappable_t test_func(Args&...);

	using result = decltype(test_func(std::declval<T&>(), std::declval<T&>()));

	static constexpr bool value = !std::is_same<result, not_swappable_t>::value;
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

template <typename T>
struct is_swappable {
	static constexpr bool value = detail::swappable_trait_impl<T>::value;
};

template <typename T>
struct is_nothrow_swappable {
	static constexpr bool value =
			detail::nothrow_swappable_trait_impl<T, detail::swappable_trait_impl<T>::value>::value;
};

} // namespace util
} // namespace mce

#endif /* UTIL_TRAITS_HPP_ */
