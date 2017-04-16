/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/scratch_pad_pool.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_SCRATCH_PAD_POOL_HPP_
#define CONTAINERS_SCRATCH_PAD_POOL_HPP_

/**
 * \file
 * Defines a generic pool to handle temporary resources.
 */

#include <mutex>
#include <stack>
#include <type_traits>

namespace mce {
namespace containers {

/// This class provides a thread-safe pool of temporary containers of type T for intermediate buffers.
/**
 * The advantage of having a pool of such containers allows them to grow to appropriate size and be reused
 * without deallocating their resources (e.g. when T is std::vector<some_type>).
 * The pool can only be used efficiently for nothrow movable types T because they are handed out and given
 * back using move semantics.
 * The type T further needs to have a clear() member function that resets it to the state it should have when
 * it is given out again to other users.
 *
 * Usage example:
 * \code{.cpp}
 * std::vector<int> input;
 * //fill input
 *
 * scratch_pad_pool<std::vector<int>> pool;
 * auto obj = pool.get();
 * std::copy_if(input.begin(),input.end(),std::back_inserter(*obj),[](int element){
 * //...
 * });
 *
 * //Work with filtered data
 *
 * //When obj goes out of scope the container is handed back to the pool.
 * \endcode
 */
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
	/// \brief RAII-wrapper for a handed out T-object that takes care of handing it back into the pool when
	/// going out of scope.
	class object {
		scratch_pad_pool<T>* pool;
		T obj;

		friend class scratch_pad_pool<T>;
		object(scratch_pad_pool<T>* pool, T&& obj) noexcept : pool(pool), obj(std::move_if_noexcept(obj)) {}

	public:
		/// Forbids copying.
		object(const object&) = delete;
		/// Forbids copying.
		object& operator=(const object&) = delete;
		/// Allows moving.
		object(object&& other) noexcept : pool(other.pool), obj(std::move_if_noexcept(other.obj)) {
			other.pool = nullptr;
		}
		/// Allows moving.
		object& operator=(object&& other) noexcept {
			pool = other.pool;
			obj = std::move_if_noexcept(other.obj);
			other.pool = nullptr;
			return *this;
		}
		/// Hands the managed T-object back to the pool it came from.
		~object() noexcept {
			if(pool) pool->give_back(std::move_if_noexcept(obj));
		}
		/// Allows access to the managed T-object.
		T& operator*() noexcept {
			return obj;
		}
		/// Allows read only access to the managed T-object.
		const T& operator*() const noexcept {
			return obj;
		}
		/// Allows access to the managed T-object.
		T* operator->() noexcept {
			return &obj;
		}
		/// Allows read only access to the managed T-object.
		const T* operator->() const noexcept {
			return &obj;
		}
	};
	/// Returns an T-object from the pool inside a object wrapper that eventually hands the T-object back.
	/**
	 * If the pool is empty a new T-object is created and handed out. When it is handed back it is put back
	 * into the pool, essentially growing the pool.
	 */
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
