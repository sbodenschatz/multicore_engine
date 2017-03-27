/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/functional.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_FUNCTIONAL_HPP_
#define UTIL_FUNCTIONAL_HPP_

#include <type_traits>
#include <utility>

namespace mce {
namespace util {

template <typename F1, typename F2>
class chain_functor {
	F1 f1;
	F2 f2;

public:
	template <typename T1, typename T2>
	chain_functor(T1&& t1, T2&& t2) : f1(std::forward<T1>(t1)), f2(std::forward<T2>(t2)) {}
	template <typename... Args>
	auto operator()(Args&&... args) {
		return f2(f1(std::forward<Args>(args)...));
	}
	template <typename... Args>
	auto operator()(Args&&... args) const {
		return f2(f1(std::forward<Args>(args)...));
	}
};

template <typename F1, typename F2>
auto chain_fn(F1&& f1, F2&& f2) {
	return chain_functor<std::decay_t<F1>, std::decay_t<F2>>(f1, f2);
}

} // namespace util
} // namespace mce

#endif /* UTIL_FUNCTIONAL_HPP_ */
