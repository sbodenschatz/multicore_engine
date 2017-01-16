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

/// Implements a spin lock on top of atomic_flag that fulfills the Lockable concept from the standard library.
/**
 * In contrast to std::mutex this lock does not relinquish the CPU time slice but waits in a tight loop for
 * acquiring the lock. This type of lock is useful for scenarios where a lock is held very shortly.
 */
class spin_lock {
	std::atomic_flag flag = ATOMIC_FLAG_INIT;

public:
	/// Waits until the lock is acquired by the calling thread.
	void lock() noexcept {
		while(flag.test_and_set(std::memory_order_acquire))
			;
	}
	/// Attempts to acquire the lock immediately but doesn't wait for the lock if already taken.
	bool try_lock() noexcept {
		return !flag.test_and_set(std::memory_order_acquire);
	}
	/// Releases the lock.
	void unlock() noexcept {
		flag.clear(std::memory_order_release);
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_SPIN_LOCK_HPP_ */
