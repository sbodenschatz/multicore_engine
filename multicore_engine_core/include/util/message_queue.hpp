/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/message_queue.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_MESSAGE_QUEUE_HPP_
#define UTIL_MESSAGE_QUEUE_HPP_

#include <mutex>
#include <condition_variable>
#include <queue>
#include <util/spin_lock.hpp>

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

// TODO Add (conditional) noexcepts
template <typename T, typename Lock = spin_lock>
class message_queue {
private:
	std::queue<T> queue;
	mutable Lock lock;
	typename detail::cond_var_mapper<Lock>::condition_variable cv;

public:
	T pop() {
		std::unique_lock<Lock> guard(lock);
		cv.wait(guard, [this] { return !queue.empty(); });
		T value = std::move(queue.front());
		queue.pop();
		return std::move(value);
	}

	bool try_pop(T& target) {
		std::lock_guard<Lock> guard(lock);
		if(queue.empty()) return false;
		target = std::move(queue.front());
		queue.pop();
		return true;
	}

	template <typename U>
	void push(U&& value) { // Forwarding reference to move value into the queue if possible
		{
			std::lock_guard<Lock> guard(lock);
			queue.push(std::forward<U>(value));
		} // Exit critical section here to avoid waking and immediately blocking thread
		cv.notify_one();
	}

	bool empty() const {
		std::lock_guard<Lock> guard(lock);
		return queue.empty();
	}

	size_t size() const {
		std::lock_guard<Lock> guard(lock);
		return queue.size();
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_MESSAGE_QUEUE_HPP_ */
