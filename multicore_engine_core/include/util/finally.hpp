/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/finally.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_FINALLY_HPP_
#define UTIL_FINALLY_HPP_

#include <utility>

namespace mce {
namespace util {

template <typename F>
class finally_t {
	F f;
	bool active = true;

public:
	explicit finally_t(const F& f) : f(f){};
	explicit finally_t(F&& f) : f(std::move(f)){};
	~finally_t() {
		if(active) f();
	}
	finally_t(const finally_t&) = delete;
	finally_t(finally_t&& other) : f(std::move(other.f)) {
		other.active = false;
	}
	finally_t& operator=(const finally_t&) = delete;
	finally_t& operator=(finally_t&& other) {
		f = std::move(other.f);
		active = true;
		other.active = false;
		return *this;
	}
};

template <typename F>
finally_t<F> finally(const F& f) {
	return finally_t<F>(f);
}

} // namespace util
} // namespace mce

#endif /* UTIL_FINALLY_HPP_ */
