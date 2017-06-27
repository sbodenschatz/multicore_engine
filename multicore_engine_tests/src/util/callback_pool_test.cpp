/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/callback_pool_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/util/callback_pool.hpp>
#include <mce/util/unused.hpp>

namespace mce {
namespace util {

TEST(util_callback_pool_test, call_lambda_void_noparams) {
	callback_pool p;
	int x = 0;
	auto f = p.allocate_function<void()>([&]() { x = 42; });
	f();
	ASSERT_TRUE(x == 42);
}

TEST(util_callback_pool_test, call_lambda_returnval) {
	callback_pool p;
	auto f = p.allocate_function<int(int)>([](int val) { return val * val; });
	int res = f(2);
	ASSERT_TRUE(res == 4);
}

TEST(util_callback_pool_test, call_lambda_ref_pass_through) {
	callback_pool p;
	auto f = p.allocate_function<int&(int& x)>([](int& val) -> int& { return val; });
	int x = 0;
	int& y = f(x);
	x = 42;
	UNUSED(x); // Silence "value never read" warning
	ASSERT_TRUE(y == 42);
}

static int test_value_1 = 0;

static void test_function_1() {
	test_value_1 = 42;
}

TEST(util_callback_pool_test, call_fptr_void_noparams) {
	test_value_1 = 0;
	callback_pool p;
	auto f = p.allocate_function<void()>(test_function_1);
	f();
	ASSERT_TRUE(test_value_1 == 42);
}

static void test_function_2(int val) {
	test_value_1 = val;
}

TEST(util_callback_pool_test, call_fptr_void_int_param) {
	test_value_1 = 0;
	callback_pool p;
	auto f = p.allocate_function<void(int)>(test_function_2);
	f(123);
	ASSERT_TRUE(test_value_1 == 123);
}

struct test_class {
	int x = 0;
	void foo() {
		x = 42;
	}
};

TEST(util_callback_pool_test, call_memfn_void) {
	test_class obj;
	callback_pool p;
	auto f = p.allocate_function<void(test_class&)>(std::mem_fn(&test_class::foo));
	f(obj);
	ASSERT_TRUE(obj.x == 42);
}

TEST(util_callback_pool_test, assign_move) {
	int x = 0;
	int y = 0;
	callback_pool p;
	auto f = p.allocate_function<void()>([&]() { x = 42; });
	auto f2 = p.allocate_function<void()>([&]() { y = 42; });
	f = std::move(f2);
	f();
	ASSERT_TRUE(x == 0);
	ASSERT_TRUE(y == 42);
}

} // namespace util
} // namespace mce
