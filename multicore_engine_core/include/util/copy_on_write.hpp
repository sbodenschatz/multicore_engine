/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/copy_on_write.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_COPY_ON_WRITE_HPP_
#define UTIL_COPY_ON_WRITE_HPP_

#include "monitor.hpp"
#include <memory>

namespace mce {
namespace util {

/// Provides a generic implementation of copy-on-write semantics based transactions.
/**
 * This class template uses #mce::util::monitor and std::shared_ptr to provide thread-save transactions using
 * copy-on-write on an object of type T. It is especially suitable for cases with infrequent writes but
 * many readers, that potentially work with the data for a long time. Cases with high writing frequencies are
 * costly because each write transaction requires a heap allocation and at least one copy of the managed
 * object. Very high read frequencies could lead to heavy lock contention of the monitor holding the current
 * version (implemented using a spin lock, because it just protects a shared_ptr copy).
 *
 * The type T must satisfy the concept CopyConstructible from the standard library.
 * For the copy_on_write instantiation to be default-constructible, T must also satisfy the requirements of
 * the concept DefaultConstructible from the standard library.
 *
 * Reading users can obtain a std::shared_ptr<T> to the current version of the managed object using
 * #get and keep the pointer as long as they don't want it to change underneath them from
 * their view. Writing users can apply modifying transactions to T encapsulated in a idempotent function
 * object using #do_transaction.
 */
template <typename T, typename Lock = spin_lock>
class copy_on_write {
	// TODO replace monitor<std::shared_ptr<T>> with std::atomic_shared_ptr<T> when available.
	monitor<std::shared_ptr<T>, Lock> ptr;

public:
	/// Deafult-constructs a copy_on_write object by default-constructing the managed object.
	copy_on_write() : ptr{std::make_shared<T>()} {}
	/// Constructs a copy_on_write object that manages the object given by shared_ptr.
	// cppcheck-suppress noExplicitConstructor
	copy_on_write(const std::shared_ptr<T>& object_to_manage) : ptr{object_to_manage} {}
	/// Constructs a copy_on_write object that manages the object given by shared_ptr.
	copy_on_write(std::shared_ptr<T>&& object_to_manage) : ptr{object_to_manage} {}
	/// The copy_on_write object itself can neither be copied nor moved.
	copy_on_write(const copy_on_write&) = delete;
	/// The copy_on_write object itself can neither be copied nor moved.
	copy_on_write& operator=(const copy_on_write&) = delete;

	/// Returns a pointer to the current version of the managed object that keeps that version available.
	/**
	 * Reading users can use this member function to obtain a shared_ptr to the current version of the managed
	 * object. The returned shared_ptr keeps the requested version available at least as long as the
	 * shared_ptr exists and is not repointed to another object.
	 */
	std::shared_ptr<const T> get() const {
		return ptr.load();
	}

	/// Applies a modifying transaction to the managed object.
	/**
	 * The given function object must be callable with a signature of void(T&) and must be idempotent.
	 * It is called on a copy of the managed object to apply the desired changes.
	 * The modified version is then atomically set as the current version if the version that was copied is
	 * still the current version. If another transaction modified the managed object in the mean time, the
	 * transaction is replayed on top of the now current version by calling the function object again with the
	 * now current version as a parameter. This process is repeated until the atomic exchange is successful.
	 * Therefore the number of calls to f depends on the number of concurrent writers.
	 */
	template <typename F>
	void do_transaction(F f) {
		auto old_ptr = ptr.load();
		auto new_ptr = std::make_shared<T>(*old_ptr);
		f(*new_ptr);
		while(!ptr.compare_exchange_strong(old_ptr, new_ptr)) {
			*new_ptr = *old_ptr;
			f(*new_ptr);
		};
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_COPY_ON_WRITE_HPP_ */
