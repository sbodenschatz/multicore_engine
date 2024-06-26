/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/monitor_test.cpp
 * Copyright 2015-2018 by Stefan Bodenschatz
 */

#include <algorithm>
#include <atomic>
#include <gtest.hpp>
#include <mce/util/monitor.hpp>
#include <mutex>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

namespace mce {
namespace util {

struct monitor_test_object {
	int data[64];
	monitor_test_object() noexcept {
		for(auto& elem : data) elem = 0;
	}
	explicit monitor_test_object(int val) noexcept {
		for(auto& elem : data) elem = val;
	}
	bool check() const {
		return std::all_of(std::begin(data), std::end(data), [&](auto val) { return val == data[0]; });
	}
	bool operator==(const monitor_test_object& other) const {
		return std::equal(std::begin(data), std::end(data), std::begin(other.data), std::end(other.data));
	}
};

TEST(util_monitor_test, thread_safety_copy_assign_convert_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, spin_lock> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val1(i << 10 | j);
				monitor = val1;
				std::this_thread::yield();
				monitor_test_object val2 = monitor;
				if(!val2.check()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_move_assign_convert_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, spin_lock> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor = monitor_test_object(i << 10 | j);
				std::this_thread::yield();
				monitor_test_object val2 = monitor;
				if(!val2.check()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_copy_store_load_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, spin_lock> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val1(i << 10 | j);
				monitor.store(val1);
				std::this_thread::yield();
				monitor_test_object val2 = monitor.load();
				if(!val2.check()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_move_store_load_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, spin_lock> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor.store(monitor_test_object(i << 10 | j));
				std::this_thread::yield();
				monitor_test_object val2 = monitor.load();
				if(!val2.check()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_copy_exchange_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, spin_lock> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val1(i << 10 | j);
				monitor_test_object val2 = monitor.exchange(val1);
				if(!val2.check()) ok = false;
				// std::this_thread::yield();
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_move_exchange_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, spin_lock> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val2 = monitor.exchange(monitor_test_object(i << 10 | j));
				if(!val2.check()) ok = false;
				// std::this_thread::yield();
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_copy_compare_exchange_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, spin_lock> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val_old;
				monitor_test_object val_new(val_old.data[0] + 1);
				while(!monitor.compare_exchange_strong(val_old, val_new)) {
					if(!val_old.check()) ok = false;
					val_new = monitor_test_object(val_old.data[0] + 1);
				}
				// std::this_thread::yield();
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	monitor_test_object val = monitor.load();
	ASSERT_TRUE(val.check());
	ASSERT_TRUE(val.data[0] == thread_count * elements_per_thread);
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_move_compare_exchange_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, spin_lock> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val_old;
				monitor_test_object val_new(val_old.data[0] + 1);
				while(!monitor.compare_exchange_strong(val_old, monitor_test_object(val_old.data[0] + 1))) {
					if(!val_old.check()) ok = false;
				}
				// std::this_thread::yield();
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	monitor_test_object val = monitor.load();
	ASSERT_TRUE(val.check());
	ASSERT_TRUE(val.data[0] == thread_count * elements_per_thread);
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_do_atomically_spin_lock) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, spin_lock> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor.do_atomically([&ok](monitor_test_object& value) {
					for(int i = 0; i < 64; ++i) {
						value.data[i]++;
					}
					if(!value.check()) ok = false;
				});
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	monitor_test_object val = monitor.load();
	ASSERT_TRUE(val.check());
	ASSERT_TRUE(val.data[0] == thread_count * elements_per_thread);
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_copy_assign_convert_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, std::mutex> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val1(i << 10 | j);
				monitor = val1;
				std::this_thread::yield();
				monitor_test_object val2 = monitor;
				if(!val2.check()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_move_assign_convert_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, std::mutex> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor = monitor_test_object(i << 10 | j);
				std::this_thread::yield();
				monitor_test_object val2 = monitor;
				if(!val2.check()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_copy_store_load_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, std::mutex> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val1(i << 10 | j);
				monitor.store(val1);
				std::this_thread::yield();
				monitor_test_object val2 = monitor.load();
				if(!val2.check()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_move_store_load_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, std::mutex> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor.store(monitor_test_object(i << 10 | j));
				std::this_thread::yield();
				monitor_test_object val2 = monitor.load();
				if(!val2.check()) ok = false;
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_copy_exchange_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, std::mutex> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val1(i << 10 | j);
				monitor_test_object val2 = monitor.exchange(val1);
				if(!val2.check()) ok = false;
				// std::this_thread::yield();
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_move_exchange_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, std::mutex> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&, i]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val2 = monitor.exchange(monitor_test_object(i << 10 | j));
				if(!val2.check()) ok = false;
				// std::this_thread::yield();
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_copy_compare_exchange_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, std::mutex> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val_old;
				monitor_test_object val_new(val_old.data[0] + 1);
				while(!monitor.compare_exchange_strong(val_old, val_new)) {
					if(!val_old.check()) ok = false;
					val_new = monitor_test_object(val_old.data[0] + 1);
				}
				// std::this_thread::yield();
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	monitor_test_object val = monitor.load();
	ASSERT_TRUE(val.check());
	ASSERT_TRUE(val.data[0] == thread_count * elements_per_thread);
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_move_compare_exchange_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, std::mutex> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor_test_object val_old;
				monitor_test_object val_new(val_old.data[0] + 1);
				while(!monitor.compare_exchange_strong(val_old, monitor_test_object(val_old.data[0] + 1))) {
					if(!val_old.check()) ok = false;
				}
				// std::this_thread::yield();
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	monitor_test_object val = monitor.load();
	ASSERT_TRUE(val.check());
	ASSERT_TRUE(val.data[0] == thread_count * elements_per_thread);
	ASSERT_TRUE(ok);
}

TEST(util_monitor_test, thread_safety_do_atomically_mutex) {
	std::vector<std::thread> threads;
	const int thread_count = 32;
	const int elements_per_thread = 1024;
	monitor<monitor_test_object, std::mutex> monitor;
	std::atomic<bool> ok{true};

	for(int i = 0; i < thread_count; i++) {
		threads.emplace_back([&]() {
			for(int j = 0; j < elements_per_thread; ++j) {
				monitor.do_atomically([&ok](monitor_test_object& value) {
					for(int i = 0; i < 64; ++i) {
						value.data[i]++;
					}
					if(!value.check()) ok = false;
				});
			}
		});
	}
	for(auto& thread : threads) {
		thread.join();
	}
	monitor_test_object val = monitor.load();
	ASSERT_TRUE(val.check());
	ASSERT_TRUE(val.data[0] == thread_count * elements_per_thread);
	ASSERT_TRUE(ok);
}

} // namespace util
} // namespace mce
