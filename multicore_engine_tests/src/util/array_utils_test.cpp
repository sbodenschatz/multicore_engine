/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/array_utils_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/util/array_utils.hpp>

namespace mce {
namespace util {

TEST(util_array_utils, make_array_test) {
	auto a = make_array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	static_assert(std::is_same<decltype(a), std::array<int, 10>>::value,
				  "Return value of make_array should have type std::array<int,10>.");
	for(size_t i = 0; i < a.size(); ++i) {
		ASSERT_EQ(i, a[i]);
	}
}

struct array_transform_test_object_1 {
	int val;
	array_transform_test_object_1(int val) : val{val} {}
};

TEST(util_array_utils, array_transform_from_array_test) {
	auto a = make_array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	auto transformed = util::array_transform<array_transform_test_object_1>(
			a, [](int x) { return array_transform_test_object_1(x); });
	for(size_t i = 0; i < a.size(); ++i) {
		ASSERT_EQ(i, transformed[i].val);
	}
}

struct array_transform_test_object_2 {
	int x;
	int y;
	array_transform_test_object_2(int x, int y) : x{x}, y{y} {}
};

TEST(util_array_utils, binary_array_transform_from_array_test) {
	auto a = make_array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	auto b = make_array(100, 101, 102, 103, 104, 105, 106, 107, 108, 109);
	auto transformed = util::array_transform<array_transform_test_object_2>(
			a, b, [](int x, int y) { return array_transform_test_object_2(x, y); });
	for(size_t i = 0; i < a.size(); ++i) {
		ASSERT_EQ(i, transformed[i].x);
		ASSERT_EQ(100 + i, transformed[i].y);
	}
}

struct array_generate_test_object_1 {
	int val;
	array_generate_test_object_1(int val) : val{val} {}
};

TEST(util_array_utils, array_generate_indexed_test) {
	auto a = util::array_generate_indexed<int, 10>([](int x) { return x; });
	for(size_t i = 0; i < a.size(); ++i) {
		ASSERT_EQ(i, a[i]);
	}
}

TEST(util_array_utils, array_generate_indexed_test_2) {
	auto a = util::array_generate_indexed<array_generate_test_object_1, 10>(
			[](int x) { return array_generate_test_object_1(x); });
	for(size_t i = 0; i < a.size(); ++i) {
		ASSERT_EQ(i, a[i].val);
	}
}

} // namespace util
} // namespace mce
