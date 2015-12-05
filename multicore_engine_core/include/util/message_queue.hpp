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

template <typename T, typename Lock = spin_lock>
class message_queue {
private:
	std::queue<T> queue;
	Lock lock;
	std::condition_variable cv;

public:
	T pop() {
		std::unique_lock<std::mutex> guard(lock);
		cv.wait(guard, [this] { return !queue.empty(); });
		T value = std::move(queue.front());
		queue.pop();
		return std::move(value);
	}

	bool try_pop(T& target) {
		std::lock_guard<std::mutex> guard(lock);
		if(queue.empty()) return false;
		target = std::move(queue.front());
		queue.pop();
		return true;
	}

	template <typename U>
	void push(U&& value) { // Forwarding reference to move value into the queue if possible
		{
			std::lock_guard<std::mutex> guard(lock);
			queue.push(std::forward<U>(value));
		} // Exit critical section here to avoid waking and immediately blocking thread
		cv.notify_one();
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_MESSAGE_QUEUE_HPP_ */
