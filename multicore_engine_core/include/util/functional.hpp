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
	chain_functor(T1&& t1, T2&& t2)
			: f1(std::forward<T1>(t1)), f2(std::forward<T2>(t2)) {}
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

/// A generic functor class that combines multiple function object types into an overload set.
template <typename... Funcs>
class overload_functor;

/// Specialization of overload_functor for the step case.
template <typename Func1, typename... Funcs>
class overload_functor<Func1, Funcs...> : public Func1, public overload_functor<Funcs...>::cur {
	using cur = overload_functor;

public:
	/// Constructs an overload_functor object for the given function objects.
	template <typename F1, typename... Fs>
	overload_functor(F1&& f1, Fs&&... fs)
			: Func1(std::forward<F1>(f1)), overload_functor<Funcs...>::cur(std::forward<Fs>(fs)...) {}

	using Func1::operator();
	using overload_functor<Funcs...>::cur::operator();
};

/// Specialization of overload_functor for the anchor case.
template <typename Func>
class overload_functor<Func> : public Func {
	using cur = Func;

public:
	using Func::operator();
};

/// Returns a function object whose operator() is overloaded by combining the given function objects.
template <typename... Funcs>
overload_functor<std::remove_reference<Funcs>...> overload(Funcs&&... fs) {
	return overload_functor<std::remove_reference<Funcs>...>(std::forward<Funcs>(fs)...);
}

} // namespace util
} // namespace mce

#endif /* UTIL_FUNCTIONAL_HPP_ */
