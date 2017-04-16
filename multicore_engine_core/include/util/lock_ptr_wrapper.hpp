/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/lock_guard_ptr_wrapper.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_LOCK_PTR_WRAPPER_HPP_
#define UTIL_LOCK_PTR_WRAPPER_HPP_

/**
 * \file
 * Defines a generic class coupling smart-pointer-based ownership with locking.
 */

#include <memory>
#include <mutex>

namespace mce {
namespace util {

/// Provides a generic wrapper that holds a lock on and participates in ownership of a lockable object.
/**
 * The type T must support parameterless member functions try_lock or lock depending on the used constructor.
 * The ownership of an object of type T is managed through a smart point template Ptr and is locked by RAII
 * lock handler Lock_Guard.
 */
template <typename T, template <typename> class Lock_Guard = std::unique_lock,
		  template <typename> class Ptr = std::shared_ptr>
class lock_ptr_wrapper {
	Ptr<T> ptr_;
	Lock_Guard<T> guard_;

public:
	/// Tries to lock the given object using T::try_lock() and if successful takes ownership of it.
	/**
	 * If the locking fails the constructed wrapper object will be empty (operator bool will return false).
	 */
	lock_ptr_wrapper(const Ptr<T>& ptr_to_lockable, std::try_to_lock_t) noexcept : ptr_{}, guard_{} {
		if(ptr_to_lockable) {
			guard_ = Lock_Guard<T>(*ptr_to_lockable, std::defer_lock);
			if(guard_.try_lock()) {
				ptr_ = ptr_to_lockable;
			}
		}
	}
	/// Locks the given object in a blocking fashion using T::lock() and takes ownership of it.
	// cppcheck-suppress noExplicitConstructor
	lock_ptr_wrapper(const Ptr<T>& ptr_to_lockable) noexcept : ptr_{}, guard_{} {
		if(ptr_to_lockable) {
			guard_ = Lock_Guard<T>(*ptr_to_lockable);
			ptr_ = ptr_to_lockable;
		}
	}
	/// Unlocks the managed object and releases ownership of it.
	~lock_ptr_wrapper() {
		if(ptr_) guard_.unlock();
	}

	/// Forbids copying because only on object can own the lock.
	lock_ptr_wrapper(const lock_ptr_wrapper&) = delete;
	/// Forbids copying because only on object can own the lock.
	lock_ptr_wrapper& operator=(const lock_ptr_wrapper&) = delete;

	/// Allows moving.
	lock_ptr_wrapper(lock_ptr_wrapper&&) noexcept = default;
	/// Allows moving.
	lock_ptr_wrapper& operator=(lock_ptr_wrapper&&) noexcept = default;

	/// Allows obtaining a raw pointer to the managed object.
	T* get() const noexcept {
		return ptr_.get();
	}
	/// Allows access to the managed object.
	T& operator*() const noexcept {
		return *ptr_;
	}
	/// Allows access to the managed object.
	T* operator->() const noexcept {
		return ptr_.operator->();
	}
	/// Allows access to the smart pointer for the managed object.
	const Ptr<T>& ptr() const noexcept {
		return ptr_;
	}
	/// Returns true if the wrapper manages an object and false otherwise.
	explicit operator bool() const noexcept {
		return ptr_.operator bool();
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_LOCK_PTR_WRAPPER_HPP_ */
