/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/monitor.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_MONITOR_HPP_
#define UTIL_MONITOR_HPP_

#include <mutex>
#include <type_traits>
#include <util/spin_lock.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4522)
#endif

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
	mutable Lock lock;

public:
	monitor() noexcept = default;
	// cppcheck-suppress noExplicitConstructor
	constexpr monitor(const T& desired) noexcept(std::is_nothrow_copy_constructible<T>::value)
			: value{desired} {}
	// cppcheck-suppress noExplicitConstructor
	constexpr monitor(T&& desired) noexcept(std::is_nothrow_move_constructible<T>::value)
			: value{std::move(desired)} {}
	monitor(const monitor&) = delete;
	monitor& operator=(const monitor&) = delete;
	monitor& operator=(const monitor&) volatile = delete;
	T operator=(const T& desired) {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
		return value;
	}
	T operator=(const T& desired) volatile {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
		return value;
	}
	T operator=(T&& desired) {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
		return value;
	}
	T operator=(T&& desired) volatile {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
		return value;
	}
	bool is_lock_free() const noexcept {
		return false;
	}
	bool is_lock_free() const volatile noexcept {
		return false;
	}
	void store(const T& desired) {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
	}
	void store(const T& desired) volatile {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
	}
	void store(T&& desired) {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
	}
	void store(T&& desired) volatile {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
	}
	T load() const noexcept(std::is_nothrow_copy_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	T load() const volatile noexcept(std::is_nothrow_copy_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	operator T() const noexcept(std::is_nothrow_copy_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	operator T() const volatile noexcept(std::is_nothrow_copy_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	T exchange(const T& desired) noexcept(
			std::is_nothrow_copy_constructible<T>::value&& std::is_nothrow_move_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		T temp = value;
		value = desired;
		return temp;
	}
	T exchange(const T& desired) volatile noexcept(
			std::is_nothrow_copy_constructible<T>::value&& std::is_nothrow_move_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		T temp = value;
		value = desired;
		return temp;
	}
	T exchange(T&& desired) noexcept(std::is_nothrow_copy_constructible<T>::value ||
									 std::is_nothrow_move_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		T temp = std::move_if_noexcept(value);
		value = std::move_if_noexcept(desired);
		return std::move_if_noexcept(temp);
	}
	T exchange(T&& desired) volatile noexcept(std::is_nothrow_copy_constructible<T>::value ||
											  std::is_nothrow_move_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		T temp = std::move_if_noexcept(value);
		value = std::move_if_noexcept(desired);
		return std::move_if_noexcept(temp);
	}

	/// desired is moved from in both cases.
	bool compare_exchange_strong(T& expected, const T& desired) {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		if(expected == value) {
			using std::swap;
			// desired is not assigned directly to value but swapped into it to let the clean up of the old
			// value run outside the lock to avoid a potential circular wait.
			swap(value, new_value);
			return true;
		} else {
			expected = value;
			return false;
		}
	}
	/// desired is moved from in both cases.
	bool compare_exchange_strong(T& expected, const T& desired) volatile {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		if(expected == value) {
			using std::swap;
			// desired is not assigned directly to value but swapped into it to let the clean up of the old
			// value run outside the lock to avoid a potential circular wait.
			swap(value, new_value);
			return true;
		} else {
			expected = value;
			return false;
		}
	}
	/// desired is moved from in both cases.
	bool compare_exchange_strong(T& expected, T&& desired) {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		if(expected == value) {
			using std::swap;
			// desired is not assigned directly to value but swapped into it to let the clean up of the old
			// value run outside the lock to avoid a potential circular wait.
			swap(value, new_value);
			return true;
		} else {
			expected = value;
			return false;
		}
	}
	/// desired is moved from in both cases.
	bool compare_exchange_strong(T& expected, T&& desired) volatile {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		if(expected == value) {
			using std::swap;
			// desired is not assigned directly to value but swapped into it to let the clean up of the old
			// value run outside the lock to avoid a potential circular wait.
			swap(value, new_value);
			return true;
		} else {
			expected = value;
			return false;
		}
	}
	template <typename F>
	void do_atomically(F&& f) noexcept(noexcept(f(std::declval<T&>()))) {
		std::lock_guard<Lock> guard(lock);
		f(value);
	}
	template <typename F>
	void do_atomically(F&& f) volatile noexcept(noexcept(f(std::declval<T&>()))) {
		std::lock_guard<Lock> guard(lock);
		f(value);
	}
};

} // namespace util
} // namespace mce

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* UTIL_MONITOR_HPP_ */
