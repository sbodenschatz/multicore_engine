/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/monitor.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_MONITOR_HPP_
#define UTIL_MONITOR_HPP_

/**
 * \file
 * Defines a generic monitor class providing thread-safe access to a (not necessarily POD) typed object.
 */

#include <mutex>
#include <type_traits>
#include <mce/util/spin_lock.hpp>
#include <mce/util/traits.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4522)
#endif

namespace mce {
namespace util {

/// This class provides atomic access for an object of type T.
/**
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
	/// Initializes a monitor by default-constructing the managed T object.
	monitor() noexcept = default;
	/// Initializes a monitor by copying the given T object into the monitor.
	// cppcheck-suppress noExplicitConstructor
	constexpr monitor(const T& desired) noexcept(std::is_nothrow_copy_constructible<T>::value)
			: value{desired} {}
	/// Initializes a monitor by moving the given T object into the monitor.
	// cppcheck-suppress noExplicitConstructor
	constexpr monitor(T&& desired) noexcept(std::is_nothrow_move_constructible<T>::value)
			: value{std::move(desired)} {}
	/// Monitors are non-copyable.
	monitor(const monitor&) = delete;
	/// Monitors are non-copyable.
	monitor& operator=(const monitor&) = delete;
	/// Monitors are non-copyable.
	monitor& operator=(const monitor&) volatile = delete;
	/// Atomically assigns a new value to the managed T object by copying the given value.
	T operator=(const T& desired) {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
		return value;
	}
	/// Atomically assigns a new value to the managed T object by copying the given value.
	T operator=(const T& desired) volatile {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
		return value;
	}
	/// Atomically assigns a new value to the managed T object by moving the given value.
	T operator=(T&& desired) {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
		return value;
	}
	/// Atomically assigns a new value to the managed T object by moving the given value.
	T operator=(T&& desired) volatile {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
		return value;
	}
	/// Returns a bool indicating if the monitor is lock-free (allways false) for compatibility with atomic.
	bool is_lock_free() const noexcept {
		return false;
	}
	/// Returns a bool indicating if the monitor is lock-free (allways false) for compatibility with atomic.
	bool is_lock_free() const volatile noexcept {
		return false;
	}
	/// Atomically stores the given value into the managed T object by copying.
	void store(const T& desired) noexcept(
			is_nothrow_swappable<T>::value&& std::is_nothrow_copy_constructible<T>::value) {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
	}
	/// Atomically stores the given value into the managed T object by copying.
	void store(const T& desired) volatile noexcept(
			is_nothrow_swappable<T>::value&& std::is_nothrow_copy_constructible<T>::value) {
		T new_value = desired;
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
	}
	/// Atomically stores the given value into the managed T object by moving.
	void store(T&& desired) noexcept(
			is_nothrow_swappable<T>::value&& std::is_nothrow_move_constructible<T>::value) {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
	}
	/// Atomically stores the given value into the managed T object by moving.
	void store(T&& desired) volatile noexcept(
			is_nothrow_swappable<T>::value&& std::is_nothrow_move_constructible<T>::value) {
		T new_value = std::move(desired);
		std::lock_guard<Lock> guard(lock);
		using std::swap;
		swap(value, new_value);
	}
	/// Atomically returns the current value of the managed object.
	T load() const noexcept(std::is_nothrow_copy_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	/// Atomically returns the current value of the managed object.
	T load() const volatile noexcept(std::is_nothrow_copy_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	/// Atomically returns the current value of the managed object.
	operator T() const noexcept(std::is_nothrow_copy_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	/// Atomically returns the current value of the managed object.
	operator T() const volatile noexcept(std::is_nothrow_copy_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		return value;
	}
	/// Atomically assigns the given value by copying to the managed object and returns the old value.
	T exchange(const T& desired) noexcept(
			std::is_nothrow_copy_constructible<T>::value&& std::is_nothrow_move_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		T temp = value;
		value = desired;
		return temp;
	}
	/// Atomically assigns the given value by copying to the managed object and returns the old value.
	T exchange(const T& desired) volatile noexcept(
			std::is_nothrow_copy_constructible<T>::value&& std::is_nothrow_move_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		T temp = value;
		value = desired;
		return temp;
	}
	/// Atomically assigns the given value by moving to the managed object and returns the old value.
	T exchange(T&& desired) noexcept(std::is_nothrow_copy_constructible<T>::value ||
									 std::is_nothrow_move_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		T temp = std::move_if_noexcept(value);
		value = std::move_if_noexcept(desired);
		return std::move_if_noexcept(temp);
	}
	/// Atomically assigns the given value by moving to the managed object and returns the old value.
	T exchange(T&& desired) volatile noexcept(std::is_nothrow_copy_constructible<T>::value ||
											  std::is_nothrow_move_constructible<T>::value) {
		std::lock_guard<Lock> guard(lock);
		T temp = std::move_if_noexcept(value);
		value = std::move_if_noexcept(desired);
		return std::move_if_noexcept(temp);
	}

	/// \brief Atomically replaces the current value of the managed object with desired if the value is still
	/// equal to expected or sets expected to the current value if the value is not equal to it.
	/**
	 * The value referenced by desired is copied in both cases.
	 * Returns true if the exchange happened and false if it didn't.
	 */
	bool compare_exchange_strong(T& expected, const T& desired) noexcept(
			is_nothrow_swappable<T>::value&& std::is_nothrow_copy_constructible<T>::value&&
					std::is_nothrow_copy_assignable<T>::value) {
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
	/// \brief Atomically replaces the current value of the managed object with desired if the value is still
	/// equal to expected or sets expected to the current value if the value is not equal to it.
	/**
	 * The value referenced by desired is copied in both cases.
	 * Returns true if the exchange happened and false if it didn't.
	 */
	bool compare_exchange_strong(T& expected, const T& desired) volatile noexcept(
			is_nothrow_swappable<T>::value&& std::is_nothrow_copy_constructible<T>::value&&
					std::is_nothrow_copy_assignable<T>::value) {
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
	/// \brief Atomically replaces the current value of the managed object with desired if the value is still
	/// equal to expected or sets expected to the current value if the value is not equal to it.
	/**
	 * The value referenced by desired is moved from in both cases.
	 * Returns true if the exchange happened and false if it didn't.
	 */
	bool compare_exchange_strong(T& expected, T&& desired) noexcept(
			is_nothrow_swappable<T>::value&& std::is_nothrow_move_constructible<T>::value&&
					std::is_nothrow_copy_assignable<T>::value) {
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
	/// \brief Atomically replaces the current value of the managed object with desired if the value is still
	/// equal to expected or sets expected to the current value if the value is not equal to it.
	/**
	 * The value referenced by desired is moved from in both cases.
	 * Returns true if the exchange happened and false if it didn't.
	 */
	bool compare_exchange_strong(T& expected, T&& desired) volatile noexcept(
			is_nothrow_swappable<T>::value&& std::is_nothrow_move_constructible<T>::value&&
					std::is_nothrow_copy_assignable<T>::value) {
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
	/// \brief Atomically executes the function object f by calling it with the current value of the managed
	/// object as the only parameter.
	/**
	 * The function object f should have a signature of void(T&) or void(const T&).
	 */
	template <typename F>
	void do_atomically(F&& f) noexcept(noexcept(f(std::declval<T&>()))) {
		std::lock_guard<Lock> guard(lock);
		f(value);
	}
	/// \brief Atomically executes the function object f by calling it with the current value of the managed
	/// object as the only parameter.
	/**
	 * The function object f should have a signature of void(T&) or void(const T&).
	 */
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
