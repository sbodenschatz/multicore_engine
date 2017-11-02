/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/locked.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_LOCKED_HPP_
#define UTIL_LOCKED_HPP_

/**
 * \file
 * Defines a generic automatic locking class.
 */

#include <mutex>
#include <type_traits>

namespace mce {
namespace util {

/// Provides a generic wrapper that allows to automatically lock all accesses to the wrapped object of type T.
/**
 * Users of the object use either operator-> or a locked_transaction token as follows:
 * \code{.cpp}
 * class foo{
 * 	void f(){}
 * 	void g(){}
 * };
 *
 * locked<foo> obj;
 * obj->f(); //Locks obj for the duration of this expression.
 *
 * {
 * 	auto t = obj.start_transaction();//Locks obj until t goes out of scope.
 * 	t->f();
 * 	t->g();
 * }// Lock from t is released here.
 * \endcode
 *
 * The type of lock used can be configured by the second template parameter and defaults to std::mutex.
 *
 * The implementation makes use of the special rule in C++ that allows chained application of overloaded
 * operator-> until a raw pointer is returned for which the built-in operator-> is applied.
 */
template <typename T, typename Sync_Object = std::mutex>
class locked {
public:
	/// Acts as a transaction token and access proxy for the managed object.
	template <typename U, typename S>
	class locked_transaction {
	private:
		U* value_ptr;
		std::unique_lock<S> lock;

	public:
		/// Saves the access pointer, locks and sync_object and ensures it is unlocked on destruction.
		locked_transaction(U* value_ptr, S& sync_object) noexcept : value_ptr(value_ptr), lock(sync_object) {}
		/// Chains to the built-in operator-> by returning a raw pointer.
		U* operator->() const noexcept {
			return value_ptr;
		}

		U& operator*() const noexcept {
			return *value_ptr;
		}
	};

private:
	mutable Sync_Object sync_object;
	T value;

public:
	/// Constructs the wrapper and the managed object by forwarding the given arguments.
	template <typename... Args>
	explicit locked(Args&&... args) noexcept(std::is_nothrow_constructible<T, Args...>::value)
			: value(std::forward<Args>(args)...) {}
	/// Forbid copying.
	locked(const locked&) = delete;
	/// Forbid copying.
	locked& operator=(const locked&) = delete;
	/// Forbid moving.
	locked(const locked&&) = delete;
	/// Forbid moving.
	locked& operator=(const locked&&) = delete;
	/// Locks the managed object for the rest of the expression and makes it accessible by chaining.
	locked_transaction<const T, Sync_Object> operator->() const noexcept {
		return locked_transaction<const T, Sync_Object>(&value, sync_object);
	}
	/// Locks the managed object for the rest of the expression and makes it accessible by chaining.
	locked_transaction<T, Sync_Object> operator->() noexcept {
		return locked_transaction<T, Sync_Object>(&value, sync_object);
	}
	/// Locks the managed object for the lifetime of the returned proxy that allows access.
	locked_transaction<const T, Sync_Object> start_transaction() const noexcept {
		return locked_transaction<const T, Sync_Object>(&value, sync_object);
	}
	/// Locks the managed object for the lifetime of the returned proxy that allows access.
	locked_transaction<T, Sync_Object> start_transaction() noexcept {
		return locked_transaction<T, Sync_Object>(&value, sync_object);
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_LOCKED_HPP_ */
