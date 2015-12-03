/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/spin_lock.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_SPIN_LOCK_HPP_
#define UTIL_SPIN_LOCK_HPP_

#include <atomic>

namespace mce {
namespace util {

class spin_lock {
	std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
	void lock() {
		while(flag.test_and_set(std::memory_order_acquire))
			;
	}
	bool try_lock() {
		return !flag.test_and_set(std::memory_order_acquire);
	}
	void unlock() {
		flag.clear(std::memory_order_release);
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_SPIN_LOCK_HPP_ */
