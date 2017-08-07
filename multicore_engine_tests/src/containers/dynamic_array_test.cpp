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
	dynamic_array_test_object_1(int x, int y) : x{x}, y{y} {}
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

TEST(containers_dynamic_array_test, construct_from_function) {
	dynamic_array<dynamic_array_test_object_1> a(10, [](size_t i) { return int(i); },
												 [](size_t i) { return int(i * i); });
	ASSERT_EQ(10, a.size());
	for(int i = 0; i < 10; ++i) {
		ASSERT_EQ(i, a[i].x);
		ASSERT_EQ(i * i, a[i].y);
		ASSERT_EQ(i, a.at(i).x);
		ASSERT_EQ(i * i, a.at(i).y);
	}
}

TEST(containers_dynamic_array_test, construct_from_initlist) {
	dynamic_array<dynamic_array_test_object_1> a({{0, 0}, {1, 1}, {2, 4}, {3, 9}, {4, 16}, {5, 25}, {6, 36}});
	ASSERT_EQ(7, a.size());
	for(int i = 0; i < 7; ++i) {
		ASSERT_EQ(i, a[i].x);
		ASSERT_EQ(i * i, a[i].y);
		ASSERT_EQ(i, a.at(i).x);
		ASSERT_EQ(i * i, a.at(i).y);
	}
}

} // namespace containers
} // namespace mce
