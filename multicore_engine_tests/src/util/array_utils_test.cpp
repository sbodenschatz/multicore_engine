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

} // namespace util
} // namespace mce
