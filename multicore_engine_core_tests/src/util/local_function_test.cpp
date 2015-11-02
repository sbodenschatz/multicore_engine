/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/util/local_function_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <util/local_function.hpp>

namespace mce {
namespace util {

BOOST_AUTO_TEST_SUITE(util)
BOOST_AUTO_TEST_SUITE(local_function_test)

BOOST_AUTO_TEST_CASE(call_lambda_void_noparams) {
	int x = 0;
	local_function<128, void()> f = [&]() { x = 42; };
	f();
	BOOST_CHECK(x == 42);
}

BOOST_AUTO_TEST_CASE(call_lambda_returnval) {
	local_function<128, int(int x)> f = [](int val) { return val * val; };
	int res = f(2);
	BOOST_CHECK(res == 4);
}

BOOST_AUTO_TEST_CASE(call_lambda_ref_pass_through) {
	local_function<128, int&(int& x)> f = [](int& val) -> int& { return val; };
	int x = 0;
	int& y = f(x);
	x = 42;
	static_cast<void>(x); // Silence "value never read" warning
	BOOST_CHECK(y == 42);
}

static int test_value_1 = 0;

void test_function_1() {
	test_value_1 = 42;
}

BOOST_AUTO_TEST_CASE(call_fptr_void_noparams) {
	test_value_1 = 0;
	local_function<128, void()> f = test_function_1;
	f();
	BOOST_CHECK(test_value_1 == 42);
}

void test_function_2(int val) {
	test_value_1 = val;
}

BOOST_AUTO_TEST_CASE(call_fptr_void_int_param) {
	test_value_1 = 0;
	local_function<128, void(int)> f = test_function_2;
	f(123);
	BOOST_CHECK(test_value_1 == 123);
}

struct test_class {
	int x = 0;
	void foo() {
		x = 42;
	}
};

BOOST_AUTO_TEST_CASE(call_memfn_void) {
	test_class obj;
	local_function<128, void(test_class&)> f = std::mem_fn(&test_class::foo);
	f(obj);
	BOOST_CHECK(obj.x == 42);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace memory
} // namespace mce
