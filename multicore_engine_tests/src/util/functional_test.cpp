/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/algorithm_test.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <cctype>
#include <functional>
#include <gtest.hpp>
#include <mce/util/functional.hpp>
#include <string>

namespace mce {
namespace util {

TEST(util_functional, chain_fn_filter_spaces) {
	std::string test = "Hello World! This is a Test";
	std::string test_out;
	std::copy_if(test.begin(), test.end(), std::back_inserter(test_out),
				 mce::util::chain_fn(static_cast<int (*)(int)>(std::isspace), std::logical_not<>()));
	std::string test_expected = "HelloWorld!ThisisaTest";
	ASSERT_TRUE(test_out == test_expected);
}

} // namespace util
} // namespace mce
