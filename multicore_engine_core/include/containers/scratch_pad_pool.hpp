/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/scratch_pad_pool.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_SCRATCH_PAD_POOL_HPP_
#define CONTAINERS_SCRATCH_PAD_POOL_HPP_

#include <deque>
#include <memory>
#include <mutex>
#include <stack>
#include <type_traits>

namespace mce {
namespace containers {

// Can only be used efficiently for nothrow movable types.
// T needs to have a clear() method.
template <typename T>
class scratch_pad_pool {
	static_assert(std::is_nothrow_move_constructible<T>::value,
				  "scratch_pad_pool<T> can only be used efficiently if T is nothrow movable.");

private:
	std::mutex pool_mutex;
	std::stack<T> pool;
	void give_back(T&& obj) noexcept {
		obj.clear();
		std::lock_guard<std::mutex> lock(pool_mutex);
		pool.push(std::move_if_noexcept(obj));
	}

public:
	class object {
		scratch_pad_pool<T>* pool;
		T obj;

	public:
		object(scratch_pad_pool<T>* pool, T&& obj) noexcept : pool(pool), obj(std::move_if_noexcept(obj)) {}
		object(const object&) = delete;
		object& operator=(const object&) = delete;
		object(object&& other) noexcept : pool(other.pool), obj(std::move_if_noexcept(other.obj)) {
			other.pool = nullptr;
		}
		object& operator=(object&& other) noexcept {
			pool = other.pool;
			obj = std::move_if_noexcept(other.obj);
			other.pool = nullptr;
			return *this;
		}
		~object() noexcept {
			if(pool) pool->give_back(std::move_if_noexcept(obj));
		}
		T& operator*() noexcept {
			return obj;
		}
		const T& operator*() const noexcept {
			return obj;
		}
		T* operator->() noexcept {
			return &obj;
		}
		const T* operator->() const noexcept {
			return &obj;
		}
	};
	object get() {
		std::lock_guard<std::mutex> lock(pool_mutex);
		if(pool.empty()) {
			return object(this, T());
		} else {
			object o(this, std::move_if_noexcept(pool.top()));
			pool.pop();
			return o;
		}
	}
};

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_SCRATCH_PAD_POOL_HPP_ */
