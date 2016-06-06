/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/util/algorithm_test.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <string>
#include <util/functional.hpp>

namespace mce {
namespace util {

BOOST_AUTO_TEST_SUITE(util)
BOOST_AUTO_TEST_SUITE(functional)

BOOST_AUTO_TEST_CASE(chain_fn_filter_spaces) {
	std::string test = "Hello World! This is a Test";
	std::string test_out;
	std::copy_if(test.begin(), test.end(), std::back_inserter(test_out),
				 mce::util::chain_fn(static_cast<int (*)(int)>(std::isspace), std::logical_not<>()));
	std::string test_expected = "HelloWorld!ThisisaTest";
	BOOST_CHECK(test_out == test_expected);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace util
} // namespace mce
