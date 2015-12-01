/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/finally.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_FINALLY_HPP_
#define UTIL_FINALLY_HPP_

namespace mce {
namespace util {

template <typename F>
class finally_t {
	F f;

public:
	finally_t(const F& f) : f{f} {};
	finally_t(F&& f) : f{std::move(f)} {};
	~finally_t() {
		f();
	}
};

template <typename F>
finally_t<F> finally(const F& f) {
	return finally_t<F>(f);
}

} // namespace util
} // namespace mce

#endif /* UTIL_FINALLY_HPP_ */
