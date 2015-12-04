/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/monitor.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_MONITOR_HPP_
#define UTIL_MONITOR_HPP_

#include <mutex>
#include <util/spin_lock.hpp>

namespace mce {
namespace util {

/**
 * This class provides atomic access for an object of type T.
 * The interface is based on that of std::atomic, but doesn't support different memory orders.
 *
 * The difference to std::atomic is, that this class doesn't require T to be TriviallyCopyable and always uses
 * a leaf-level lock for synchronization instead of atomic operations to remove the TriviallyCopyable
 * requirement.
 * The type of lock to use can be specified using the Lock template parameter, which needs to implement the
 * Lockable concept.
 * The memory ordering provided by the monitor depends on the ordering provided by the given Lock type.
 *
 * Another interface difference to atomic is, that because the T for a monitor may typically be more complex
 * than those used with atomic, monitor tries to reduce copies by taking parameters (e.g. desired values) by
 * const L-value reference or by R-value reference instead of by value.
 */
// TODO: Add (conditional) noexcepts.
template <typename T, typename Lock = spin_lock>
class monitor {
	T value;
	Lock lock;

public:
	monitor() noexcept = default;
	constexpr monitor(const T& desired) : value{desired} {}
	constexpr monitor(T&& desired) : value{std::move(desired)} {}
	monitor(const monitor&) = delete;
	monitor& operator=(const monitor&) = delete;
	monitor& operator=(const monitor&) volatile = delete;
	T operator=(const T& desired) {
		std::lock_guard<Lock> guard(lock);
		value = desired;
		return value;
	}
	T operator=(const T& desired) volatile {
		std::lock_guard<Lock> guard(lock);
		value = desired;
		return value;
	}
	T operator=(T&& desired) {
		std::lock_guard<Lock> guard(lock);
		value = std::move(desired);
		return value;
	}
	T operator=(T&& desired) volatile {
		std::lock_guard<Lock> guard(lock);
		value = std::move(desired);
		return value;
	}
	bool is_lock_free() const {
		return false;
	}
	bool is_lock_free() const volatile {
		return false;
	}
	void store(const T& desired) {
		std::lock_guard<Lock> guard(lock);
		value = desired;
	}
	void store(const T& desired) volatile {
		std::lock_guard<Lock> guard(lock);
		value = desired;
	}
	void store(T&& desired) {
		std::lock_guard<Lock> guard(lock);
		value = std::move(desired);
	}
	void store(T&& desired) volatile {
		std::lock_guard<Lock> guard(lock);
		value = std::move(desired);
	}
	T load() const {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	T load() const volatile {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	operator T() const {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	operator T() const volatile {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	T exchange(const T& desired) {
		std::lock_guard<Lock> guard(lock);
		T temp = value;
		value = desired;
		return temp;
	}
	T exchange(const T& desired) volatile {
		std::lock_guard<Lock> guard(lock);
		T temp = value;
		value = desired;
		return temp;
	}
	T exchange(T&& desired) {
		std::lock_guard<Lock> guard(lock);
		T temp = std::move_if_noexcept(value);
		value = std::move_if_noexcept(desired);
		return std::move_if_noexcept(temp);
	}
	T exchange(T&& desired) volatile {
		std::lock_guard<Lock> guard(lock);
		T temp = std::move_if_noexcept(value);
		value = std::move_if_noexcept(desired);
		return std::move_if_noexcept(temp);
	}

	bool compare_exchange_strong(T& expected, const T& desired) {
		std::lock_guard<Lock> guard(lock);
		if(expected == value) {
			value = desired;
			return true;
		} else {
			expected = value;
			return false;
		}
	}
	bool compare_exchange_strong(T& expected, const T& desired) volatile {
		std::lock_guard<Lock> guard(lock);
		if(expected == value) {
			value = desired;
			return true;
		} else {
			expected = value;
			return false;
		}
	}
	bool compare_exchange_strong(T& expected, T&& desired) {
		std::lock_guard<Lock> guard(lock);
		if(expected == value) {
			value = std::move(desired);
			return true;
		} else {
			expected = value;
			return false;
		}
	}
	bool compare_exchange_strong(T& expected, T&& desired) volatile {
		std::lock_guard<Lock> guard(lock);
		if(expected == value) {
			value = std::move(desired);
			return true;
		} else {
			expected = value;
			return false;
		}
	}
	/*
	bool compare_exchange_weak(T& expected, const T& desired) {
		std::unique_lock<Lock> guard(lock, std::defer_lock);
		if(guard.try_lock()) {
			if(expected == value) {
				value = desired;
				return true;
			} else {
				expected = value;
				return false;
			}
		} else {
			// Problem: can't update expected here
			return false;
		}
	}
	bool compare_exchange_weak(T& expected, const T& desired) volatile {
		std::unique_lock<Lock> guard(lock, std::defer_lock);
		if(guard.try_lock()) {
			if(expected == value) {
				value = desired;
				return true;
			} else {
				expected = value;

				return false;
			}
		} else {
			// Problem: can't update expected here
			return false;
		}
	}
	bool compare_exchange_weak(T& expected, T&& desired) {
		std::unique_lock<Lock> guard(lock, std::defer_lock);
		if(guard.try_lock()) {
			if(expected == value) {
				value = std::move(desired);
				return true;
			} else {
				expected = value;
				return false;
			}
		} else {
			// Problem: can't update expected here
			return false;
		}
	}
	bool compare_exchange_weak(T& expected, T&& desired) volatile {
		std::unique_lock<Lock> guard(lock, std::defer_lock);
		if(guard.try_lock()) {
			if(expected == value) {
				value = std::move(desired);
				return true;
			} else {
				expected = value;
				return false;
			}
		} else {
			// Problem: can't update expected here
			return false;
		}
	}
	*/
	template <typename F>
	void do_atomically(F f) {
		std::lock_guard<Lock> guard(lock);
		f(value);
	}
	template <typename F>
	void do_atomically(F f) volatile {
		std::lock_guard<Lock> guard(lock);
		f(value);
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_MONITOR_HPP_ */
