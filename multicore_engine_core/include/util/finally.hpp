/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/finally.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_FINALLY_HPP_
#define UTIL_FINALLY_HPP_

#include <utility>

namespace mce {
namespace util {

/// Implements the scope-exit-guard logic for #mce::util::finally.
template <typename F>
class finally_t {
	F f;
	bool active = true;

public:
	/// Creates the scope-exit-guard from the given function object.
	explicit finally_t(const F& f) : f(f){};
	/// Creates the scope-exit-guard from the given function object.
	explicit finally_t(F&& f) : f(std::move(f)){};
	/// Executes the saved function object if the object is not moved from.
	~finally_t() {
		if(active) f();
	}
	/// Forbids copying.
	finally_t(const finally_t&) = delete;
	/// Move constructor for finally_t.
	finally_t(finally_t&& other) : f(std::move(other.f)) {
		other.active = false;
	}
	/// Forbids copying.
	finally_t& operator=(const finally_t&) = delete;
	/// Move constructor for finally_t.
	finally_t& operator=(finally_t&& other) {
		f = std::move(other.f);
		active = true;
		other.active = false;
		return *this;
	}
};

/// Returns a scope-exit-guard that executes the given function object when it goes out of scope.
template <typename F>
finally_t<F> finally(const F& f) {
	return finally_t<F>(f);
}

} // namespace util
} // namespace mce

#endif /* UTIL_FINALLY_HPP_ */
