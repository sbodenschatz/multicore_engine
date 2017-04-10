/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/util/message_queue_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/container/flat_set.hpp>
#include <gtest.hpp>
#include <mutex>
#include <thread>
#include <util/message_queue.hpp>
#include <util/spin_lock.hpp>
#include <utility>
#include <vector>

namespace mce {
namespace util {

TEST(util_message_queue_test, thread_safety_push_pop_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 128;
	message_queue<std::pair<int, int>, spin_lock> queue;
	boost::container::flat_set<std::pair<int, int>> actual;
	boost::container::flat_set<std::pair<int, int>> expected;
	actual.reserve(thread_count * elements_per_thread);
	expected.reserve(thread_count * elements_per_thread);
	for(int i = 0; i < thread_count; i++) {
		for(int j = 0; j < elements_per_thread; ++j) {
			expected.emplace(i, j);
		}
	}
	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				queue.push(std::make_pair(i, j));
			}
		});
	}
	for(int i = 0; i < thread_count * elements_per_thread; ++i) {
		actual.emplace(queue.pop());
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(queue.empty());
	ASSERT_TRUE(actual == expected);
}

TEST(util_message_queue_test, thread_safety_push_pop_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 128;
	message_queue<std::pair<int, int>, std::mutex> queue;
	boost::container::flat_set<std::pair<int, int>> actual;
	boost::container::flat_set<std::pair<int, int>> expected;
	actual.reserve(thread_count * elements_per_thread);
	expected.reserve(thread_count * elements_per_thread);
	for(int i = 0; i < thread_count; i++) {
		for(int j = 0; j < elements_per_thread; ++j) {
			expected.emplace(i, j);
		}
	}
	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				queue.push(std::make_pair(i, j));
			}
		});
	}
	for(int i = 0; i < thread_count * elements_per_thread; ++i) {
		actual.emplace(queue.pop());
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(queue.empty());
	ASSERT_TRUE(actual == expected);
}

TEST(util_message_queue_test, thread_safety_push_try_pop_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 128;
	message_queue<std::pair<int, int>, spin_lock> queue;
	boost::container::flat_set<std::pair<int, int>> actual;
	boost::container::flat_set<std::pair<int, int>> expected;
	actual.reserve(thread_count * elements_per_thread);
	expected.reserve(thread_count * elements_per_thread);
	for(int i = 0; i < thread_count; i++) {
		for(int j = 0; j < elements_per_thread; ++j) {
			expected.emplace(i, j);
		}
	}
	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				queue.push(std::make_pair(i, j));
			}
		});
	}
	for(int i = 0; i < thread_count * elements_per_thread;) {
		std::pair<int, int> elem;
		if(queue.try_pop(elem)) {
			actual.emplace(elem);
			++i;
		}
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(queue.empty());
	ASSERT_TRUE(actual == expected);
}

TEST(util_message_queue_test, thread_safety_push_try_pop_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 128;
	message_queue<std::pair<int, int>, std::mutex> queue;
	boost::container::flat_set<std::pair<int, int>> actual;
	boost::container::flat_set<std::pair<int, int>> expected;
	actual.reserve(thread_count * elements_per_thread);
	expected.reserve(thread_count * elements_per_thread);
	for(int i = 0; i < thread_count; i++) {
		for(int j = 0; j < elements_per_thread; ++j) {
			expected.emplace(i, j);
		}
	}
	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				queue.push(std::make_pair(i, j));
			}
		});
	}
	for(int i = 0; i < thread_count * elements_per_thread;) {
		std::pair<int, int> elem;
		if(queue.try_pop(elem)) {
			actual.emplace(elem);
			++i;
		}
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(queue.empty());
	ASSERT_TRUE(actual == expected);
}

} // namespace util
} // namespace mce
