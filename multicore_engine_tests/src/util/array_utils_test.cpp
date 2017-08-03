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
	int c = 0;
	auto transformed = util::array_transform<array_transform_test_object_1>(
			a, [&c](int x) { return array_transform_test_object_1(++c + x); });
	ASSERT_EQ(1, transformed[0].val);
	ASSERT_EQ(3, transformed[1].val);
	ASSERT_EQ(5, transformed[2].val);
	ASSERT_EQ(7, transformed[3].val);
	ASSERT_EQ(9, transformed[4].val);
	ASSERT_EQ(11, transformed[5].val);
	ASSERT_EQ(13, transformed[6].val);
	ASSERT_EQ(15, transformed[7].val);
	ASSERT_EQ(17, transformed[8].val);
	ASSERT_EQ(19, transformed[9].val);
}

} // namespace util
} // namespace mce
