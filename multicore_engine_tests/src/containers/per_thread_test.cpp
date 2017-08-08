/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/containers/per_thread_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/containers/per_thread.hpp>

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

} // namespace containers
} // namespace mce
