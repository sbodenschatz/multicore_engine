/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/spin_lock_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <atomic>
#include <cstdint>
#include <gtest.hpp>
#include <mce/util/spin_lock.hpp>
#include <mutex>
#include <thread>
#include <vector>

namespace mce {
namespace util {

TEST(util_spin_lock_test, thread_safety_lock) {
	std::vector<std::thread> threads;
	std::vector<uint64_t> test;
	mce::util::spin_lock lock;
	const int thread_count = 256;
	const int elements_per_thread = 1024;

	test.reserve(thread_count * static_cast<size_t>(elements_per_thread));
	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				std::lock_guard<mce::util::spin_lock> guard(lock);
				test.emplace_back((static_cast<uint64_t>(i) << 32) | static_cast<uint64_t>(j));
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(test.size() == thread_count * elements_per_thread);
}
TEST(util_spin_lock_test, thread_safety_try_lock) {
	std::vector<std::thread> threads;
	std::vector<uint64_t> test;
	mce::util::spin_lock lock;
	const int thread_count = 256;
	const int elements_per_thread = 102400;
	std::atomic<size_t> successful_insertions{0};

	test.reserve(thread_count * static_cast<size_t>(elements_per_thread));
	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				if(lock.try_lock()) {
					successful_insertions++;
					test.emplace_back((static_cast<uint64_t>(i) << 32) | static_cast<uint64_t>(j));
					lock.unlock();
				}
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(test.size() == successful_insertions.load());
}

} // namespace util
} // namespace mce
