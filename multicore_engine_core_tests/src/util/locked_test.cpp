/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/util/locked_test.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <atomic>
#include <boost/test/unit_test.hpp>
#include <mutex>
#include <thread>
#include <util/locked.hpp>
#include <utility>
#include <vector>

namespace mce {
namespace util {

BOOST_AUTO_TEST_SUITE(util)
BOOST_AUTO_TEST_SUITE(locked_test)

BOOST_AUTO_TEST_CASE(thread_safety_stress_test) {
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
		threads.emplace_back([&, i]() {
			for(int j = 0; j < iterations_per_thread; ++j) {
				if(!locked_test_object->do_test()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	BOOST_CHECK(ok);
	BOOST_CHECK(locked_test_object->a == thread_count * iterations_per_thread + 1);
	BOOST_CHECK(locked_test_object->b == thread_count * iterations_per_thread);
}

BOOST_AUTO_TEST_CASE(mutex_mock_test) {
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
	BOOST_CHECK(mocked_mutex_counter == 0);
	BOOST_CHECK(locked_test_object->test());
	BOOST_CHECK(mocked_mutex_counter == 0);
}

BOOST_AUTO_TEST_CASE(mutex_mock_test_explicit_transaction) {
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
	BOOST_CHECK(mocked_mutex_counter == 0);
	{
		auto transaction = locked_test_object.start_transaction();
		BOOST_CHECK(mocked_mutex_counter == 1);
	}
	BOOST_CHECK(mocked_mutex_counter == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
}
}
