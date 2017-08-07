/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/containers/dynamic_array_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/containers/dynamic_array.hpp>

namespace mce {
namespace containers {

struct dynamic_array_test_object_1 {
	int x;
	int y;
};

TEST(containers_dynamic_array_test, construct_from_value) {
	dynamic_array<dynamic_array_test_object_1> a(10, {123, 456});
	ASSERT_EQ(10, a.size());
	for(int i = 0; i < 10; ++i) {
		ASSERT_EQ(123, a[i].x);
		ASSERT_EQ(456, a[i].y);
		ASSERT_EQ(123, a.at(i).x);
		ASSERT_EQ(456, a.at(i).y);
	}
}

} // namespace containers
} // namespace mce
