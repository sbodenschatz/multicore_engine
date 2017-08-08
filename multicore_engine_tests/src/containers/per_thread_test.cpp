/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/containers/per_thread_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/containers/per_thread.hpp>
#include <chrono>

namespace mce {
namespace containers {

struct per_thread_test_object_1 {
	int index;
	int val;
	int sqr;
};

TEST(containers_per_thread_test, construction) {
	per_thread<per_thread_test_object_1> pt(32, index_param_tag<int>{}, 42,
											generator_param([](size_t i) { return int(i * i); }));
	for(size_t i = 0; i < 32; ++i) {
		ASSERT_EQ(i, (pt.begin() + i)->index);
		ASSERT_EQ(42, (pt.begin() + i)->val);
		ASSERT_EQ(i * i, (pt.begin() + i)->sqr);
	}
}

TEST(containers_per_thread_test, thread_index_consistency) {
	constexpr int num_threads = 128;
	per_thread<per_thread_test_object_1> pt(num_threads, index_param_tag<int>{}, 42,
											generator_param([](size_t i) { return int(i * i); }));
	bool res[num_threads];
	int indices[num_threads];
	std::vector<std::thread> threads;
	for(int i = 0; i < num_threads; ++i) {
		threads.emplace_back([i, &res, &pt, &indices]() {
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1ms);
			auto index = pt.slot_index();
			indices[i] = int(index);
			for(int j = 0; j < 1024; ++j) {
				std::this_thread::yield();
				if(index != pt.slot_index()) {
					res[i] = false;
					return;
				}
				res[i] = true;
			}
		});
	}
	for(auto& t : threads) {
		t.join();
	}
	ASSERT_TRUE(std::all_of(std::begin(res), std::end(res), [](bool r) { return r; }));
	auto ib = std::begin(indices);
	auto ie = std::end(indices);
	std::sort(ib, ie);
	auto dup = std::adjacent_find(ib, ie);
	ASSERT_EQ(ie, dup);
}

} // namespace containers
} // namespace mce
