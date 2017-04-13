/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/local_function_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <util/local_function.hpp>
#include <util/unused.hpp>

namespace mce {
namespace util {

TEST(util_local_function_test, call_lambda_void_noparams) {
	int x = 0;
	local_function<128, void()> f = [&]() { x = 42; };
	f();
	ASSERT_TRUE(x == 42);
}

TEST(util_local_function_test, call_lambda_returnval) {
	local_function<128, int(int x)> f = [](int val) { return val * val; };
	int res = f(2);
	ASSERT_TRUE(res == 4);
}

TEST(util_local_function_test, call_lambda_ref_pass_through) {
	local_function<128, int&(int& x)> f = [](int& val) -> int& { return val; };
	int x = 0;
	int& y = f(x);
	x = 42;
	UNUSED(x); // Silence "value never read" warning
	ASSERT_TRUE(y == 42);
}

static int test_value_1 = 0;

void test_function_1() {
	test_value_1 = 42;
}

TEST(util_local_function_test, call_fptr_void_noparams) {
	test_value_1 = 0;
	local_function<128, void()> f = test_function_1;
	f();
	ASSERT_TRUE(test_value_1 == 42);
}

void test_function_2(int val) {
	test_value_1 = val;
}

TEST(util_local_function_test, call_fptr_void_int_param) {
	test_value_1 = 0;
	local_function<128, void(int)> f = test_function_2;
	f(123);
	ASSERT_TRUE(test_value_1 == 123);
}

struct test_class {
	int x = 0;
	void foo() {
		x = 42;
	}
};

TEST(util_local_function_test, call_memfn_void) {
	test_class obj;
	local_function<128, void(test_class&)> f = std::mem_fn(&test_class::foo);
	f(obj);
	ASSERT_TRUE(obj.x == 42);
}

TEST(util_local_function_test, assign_value) {
	int x = 0;
	int y = 0;
	local_function<128, void()> f = [&]() { x = 42; };
	f = [&]() { y = 42; };
	f();
	ASSERT_TRUE(x == 0);
	ASSERT_TRUE(y == 42);
}

TEST(util_local_function_test, assign_copy) {
	int x = 0;
	int y = 0;
	local_function<128, void()> f = [&]() { x = 42; };
	local_function<128, void()> f2 = [&]() { y = 42; };
	f = f2;
	f();
	ASSERT_TRUE(x == 0);
	ASSERT_TRUE(y == 42);
}

TEST(util_local_function_test, assign_move) {
	int x = 0;
	int y = 0;
	local_function<128, void()> f = [&]() { x = 42; };
	local_function<128, void()> f2 = [&]() { y = 42; };
	f = std::move(f2);
	f();
	ASSERT_TRUE(x == 0);
	ASSERT_TRUE(y == 42);
}

TEST(util_local_function_test, copy_construct_different_size) {
	int x = 0;
	local_function<128, void()> f = [&]() { x = 42; };
	local_function<256, void()> f2 = f;
	f2();
	ASSERT_TRUE(x == 42);
}

TEST(util_local_function_test, move_construct_different_size) {
	int x = 0;
	local_function<128, void()> f = [&]() { x = 42; };
	local_function<256, void()> f2 = std::move(f);
	f2();
	ASSERT_TRUE(x == 42);
}

TEST(util_local_function_test, copy_assign_different_size) {
	int x = 0;
	local_function<128, void()> f = [&]() { x = 42; };
	local_function<256, void()> f2 = []() {};
	f2 = f;
	f2();
	ASSERT_TRUE(x == 42);
}

TEST(util_local_function_test, move_assign_different_size) {
	int x = 0;
	local_function<128, void()> f = [&]() { x = 42; };
	local_function<256, void()> f2 = []() {};
	f2 = std::move(f);
	f2();
	ASSERT_TRUE(x == 42);
}

} // namespace util
} // namespace mce
