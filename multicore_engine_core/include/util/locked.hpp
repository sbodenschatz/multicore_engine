/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/locked.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_LOCKED_HPP_
#define UTIL_LOCKED_HPP_

#include <mutex>

namespace mce {
namespace util {

template <typename T, typename Sync_Object = std::mutex>
class locked {
public:
	template <typename U, typename S>
	class locked_transaction {
	private:
		U* value_ptr;
		std::unique_lock<S> lock;

	public:
		locked_transaction(U* value_ptr, S& sync_object) : value_ptr(value_ptr), lock(sync_object) {}
		U* operator->() const {
			return value_ptr;
		}
	};

private:
	mutable Sync_Object sync_object;
	T value;

public:
	template <typename... Args>
	locked(Args&&... args) : value(args...) {}
	locked(const locked&) = delete;
	locked& operator=(const locked&) = delete;
	locked(const locked&&) = delete;
	locked& operator=(const locked&&) = delete;
	locked_transaction<const T, Sync_Object> operator->() const {
		return locked_transaction<const T, Sync_Object>(&value, sync_object);
	}
	locked_transaction<T, Sync_Object> operator->() {
		return locked_transaction<T, Sync_Object>(&value, sync_object);
	}
	locked_transaction<const T, Sync_Object> start_transaction() const {
		return locked_transaction<const T, Sync_Object>(&value, sync_object);
	}
	locked_transaction<T, Sync_Object> start_transaction() {
		return locked_transaction<T, Sync_Object>(&value, sync_object);
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_LOCKED_HPP_ */
