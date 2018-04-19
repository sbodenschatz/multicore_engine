/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/locked_test.cpp
 * Copyright 2016-2018 by Stefan Bodenschatz
 */

#include <atomic>
#include <gtest.hpp>
#include <mce/util/locked.hpp>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace mce {
namespace util {

TEST(util_locked_test, thread_safety_stress_test) {
	struct locked_thread_safety_stress_test_object {
		uint64_t a = 1;
		uint64_t b = 0;
		bool do_test() {
			if(a - 1 != b) return false;
			a++;
			std::this_thread::yield();
			b++;
			if(a - 1 != b) return false;
			return true;
		}
	};
	locked<locked_thread_safety_stress_test_object> locked_test_object;
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int iterations_per_thread = 1024;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&]() {
			for(int j = 0; j < iterations_per_thread; ++j) {
				if(!locked_test_object->do_test()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
	ASSERT_TRUE(locked_test_object->a == thread_count * iterations_per_thread + 1);
	ASSERT_TRUE(locked_test_object->b == thread_count * iterations_per_thread);
}

TEST(util_locked_test, mutex_mock_test) {
	static int mocked_mutex_counter = 0;
	struct mocked_mutex {
		void lock() {
			mocked_mutex_counter++;
		}
		void unlock() {
			mocked_mutex_counter--;
		}
	};
	struct mocked_value_object {
		bool test() {
			return mocked_mutex_counter == 1;
		}
	};
	locked<mocked_value_object, mocked_mutex> locked_test_object;
	ASSERT_TRUE(mocked_mutex_counter == 0);
	ASSERT_TRUE(locked_test_object->test());
	ASSERT_TRUE(mocked_mutex_counter == 0);
}

TEST(util_locked_test, mutex_mock_test_explicit_transaction) {
	static int mocked_mutex_counter = 0;
	struct mocked_mutex {
		void lock() {
			mocked_mutex_counter++;
		}
		void unlock() {
			mocked_mutex_counter--;
		}
	};
	struct mocked_value_object {};
	locked<mocked_value_object, mocked_mutex> locked_test_object;
	ASSERT_TRUE(mocked_mutex_counter == 0);
	{
		auto transaction = locked_test_object.start_transaction();
		ASSERT_TRUE(mocked_mutex_counter == 1);
	}
	ASSERT_TRUE(mocked_mutex_counter == 0);
}
}
}
