/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/lock_guard_ptr_wrapper.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_LOCK_PTR_WRAPPER_HPP_
#define UTIL_LOCK_PTR_WRAPPER_HPP_

#include <memory>
#include <mutex>

namespace mce {
namespace util {

template <typename T, template <typename> class Lock_Guard = std::unique_lock,
		  template <typename> class Ptr = std::shared_ptr>
class lock_ptr_wrapper {
	Ptr<T> ptr_;
	Lock_Guard<T> guard_;

public:
	lock_ptr_wrapper(const Ptr<T>& ptr_to_lockable, std::try_to_lock_t) noexcept : ptr_{}, guard_{} {
		if(ptr_to_lockable) {
			guard_ = Lock_Guard<T>(*ptr_to_lockable, std::defer_lock);
			if(guard_.try_lock()) {
				ptr_ = ptr_to_lockable;
			}
		}
	}
	// cppcheck-suppress noExplicitConstructor
	lock_ptr_wrapper(const Ptr<T>& ptr_to_lockable) noexcept : ptr_{}, guard_{} {
		if(ptr_to_lockable) {
			guard_ = Lock_Guard<T>(*ptr_to_lockable);
			ptr_ = ptr_to_lockable;
		}
	}
	~lock_ptr_wrapper() {
		if(ptr_) guard_.unlock();
	}

	lock_ptr_wrapper(const lock_ptr_wrapper&) = delete;
	lock_ptr_wrapper& operator=(const lock_ptr_wrapper&) = delete;

	lock_ptr_wrapper(lock_ptr_wrapper&&) noexcept = default;
	lock_ptr_wrapper& operator=(lock_ptr_wrapper&&) noexcept = default;

	T* get() const noexcept {
		return ptr_.get();
	}
	T& operator*() const noexcept {
		return *ptr_;
	}
	T* operator->() const noexcept {
		return ptr_.operator->();
	}
	Ptr<T>& ptr() noexcept {
		return ptr_;
	}
	const Ptr<T>& ptr() const noexcept {
		return ptr_;
	}
	explicit operator bool() const noexcept {
		return ptr_.operator bool();
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_LOCK_PTR_WRAPPER_HPP_ */
