/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/message_queue.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_MESSAGE_QUEUE_HPP_
#define UTIL_MESSAGE_QUEUE_HPP_

/**
 * \file
 * Defines a generic thread-safe queue.
 */

#include <condition_variable>
#include <mce/util/spin_lock.hpp>
#include <mutex>
#include <queue>
#include <type_traits>

namespace mce {
namespace util {
namespace detail {

template <typename Lock>
struct cond_var_mapper {
	typedef std::condition_variable_any condition_variable;
};

template <>
struct cond_var_mapper<std::mutex> {
	typedef std::condition_variable condition_variable;
};

} // namespace detail

/// Provides a generic thread safe FIFO queue for communication between threads.
/**
 * The queue contains arbitrarily many objects of type T. The type T must be movable.
 * The optional template parameter can be used to specify what lock type to use.
 * By default a #mce::util::spin_lock is used.
 */
template <typename T, typename Lock = spin_lock>
class message_queue {
private:
	std::queue<T> queue;
	mutable Lock lock;
	typename detail::cond_var_mapper<Lock>::condition_variable cv;

public:
	/// Takes the element from the front of the queue and blocks if no element is available.
	T pop() noexcept(std::is_nothrow_move_assignable<T>::value) {
		std::unique_lock<Lock> guard(lock);
		cv.wait(guard, [this] { return !queue.empty(); });
		T value = std::move(queue.front());
		queue.pop();
		return std::move(value);
	}

	/// \brief If an element is available at the front of the queue it is assigned to target and true is
	/// returned, otherwise false is returned.
	bool try_pop(T& target) noexcept(std::is_nothrow_move_assignable<T>::value) {
		std::lock_guard<Lock> guard(lock);
		if(queue.empty()) return false;
		target = std::move(queue.front());
		queue.pop();
		return true;
	}

	/// Adds the given object to the end of the queue.
	/**
	 * Uses a forwarding reference for the parameter to use move semantics, if applicable based on the value
	 * category of the parameter.
	 */
	template <typename U>
	void push(U&& value) { // Forwarding reference to move value into the queue if possible, not noexcept
						   // because push might throw bad_alloc
		{
			std::lock_guard<Lock> guard(lock);
			queue.push(std::forward<U>(value));
		} // Exit critical section here to avoid waking and immediately blocking thread
		cv.notify_one();
	}

	/// Determines if the queue is currently empty.
	bool empty() const noexcept {
		std::lock_guard<Lock> guard(lock);
		return queue.empty();
	}

	/// Returns the number of objects currently in the queue
	size_t size() const noexcept {
		std::lock_guard<Lock> guard(lock);
		return queue.size();
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_MESSAGE_QUEUE_HPP_ */
