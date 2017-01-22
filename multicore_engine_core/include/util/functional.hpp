/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/functional.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_FUNCTIONAL_HPP_
#define UTIL_FUNCTIONAL_HPP_

#include <type_traits>
#include <utility>

namespace mce {
namespace util {

/// A generic functor class that allows chaining two other functors that is used by chain_fn(F1&&,F2&&).
template <typename F1, typename F2>
class chain_functor {
	F1 f1;
	F2 f2;

public:
	/// Constructs the functor from two forwarded functor objects to be chained together.
	template <typename T1, typename T2>
	chain_functor(T1&& t1, T2&& t2) : f1(std::forward<T1>(t1)), f2(std::forward<T2>(t2)) {}
	/// Calls the first functor with forwarded args and then calls the second functor with the result.
	template <typename... Args>
	auto operator()(Args&&... args) {
		return f2(f1(std::forward<Args>(args)...));
	}
	/// Calls the first functor with forwarded args and then calls the second functor with the result.
	template <typename... Args>
	auto operator()(Args&&... args) const {
		return f2(f1(std::forward<Args>(args)...));
	}
};

/// Higher order function that returns a functor that chains the two given functors.
template <typename F1, typename F2>
auto chain_fn(F1&& f1, F2&& f2) {
	return chain_functor<std::decay_t<F1>, std::decay_t<F2>>(f1, f2);
}

} // namespace util
} // namespace mce

#endif /* UTIL_FUNCTIONAL_HPP_ */
