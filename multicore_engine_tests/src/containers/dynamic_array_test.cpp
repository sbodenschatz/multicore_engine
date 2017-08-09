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

TEST(containers_dynamic_array_test, construct_from_args) {
	dynamic_array<dynamic_array_test_object_1> a(10, 123, 456);
	ASSERT_EQ(10, a.size());
	for(int i = 0; i < 10; ++i) {
		ASSERT_EQ(123, a[i].x);
		ASSERT_EQ(456, a[i].y);
		ASSERT_EQ(123, a.at(i).x);
		ASSERT_EQ(456, a.at(i).y);
	}
}

TEST(containers_dynamic_array_test, construct_from_args_index_tag) {
	dynamic_array<dynamic_array_test_object_1> a(10, index_param_tag<int>{}, 456);
	ASSERT_EQ(10, a.size());
	for(int i = 0; i < 10; ++i) {
		ASSERT_EQ(i, a[i].x);
		ASSERT_EQ(456, a[i].y);
		ASSERT_EQ(i, a.at(i).x);
		ASSERT_EQ(456, a.at(i).y);
	}
}

TEST(containers_dynamic_array_test, construct_from_function) {
	dynamic_array<dynamic_array_test_object_1> a(10, generator_param([](size_t i) { return int(i); }),
												 generator_param([](size_t i) { return int(i * i); }));
	ASSERT_EQ(10, a.size());
	for(int i = 0; i < 10; ++i) {
		ASSERT_EQ(i, a[i].x);
		ASSERT_EQ(i * i, a[i].y);
		ASSERT_EQ(i, a.at(i).x);
		ASSERT_EQ(i * i, a.at(i).y);
	}
}

TEST(containers_dynamic_array_test, construct_tags_combined) {
	dynamic_array<dynamic_array_test_object_1> a(10, index_param_tag<int>{},
												 generator_param([](size_t i) { return int(i * i); }));
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

TEST(containers_dynamic_array_test, copy_construct) {
	dynamic_array<dynamic_array_test_object_1> o_a(
			{{0, 0}, {1, 1}, {2, 4}, {3, 9}, {4, 16}, {5, 25}, {6, 36}});

	auto a = o_a;

	ASSERT_EQ(7, a.size());
	for(int i = 0; i < 7; ++i) {
		ASSERT_EQ(i, a[i].x);
		ASSERT_EQ(i * i, a[i].y);
		ASSERT_EQ(i, a.at(i).x);
		ASSERT_EQ(i * i, a.at(i).y);
	}
	ASSERT_EQ(7, o_a.size());
	for(int i = 0; i < 7; ++i) {
		ASSERT_EQ(i, o_a[i].x);
		ASSERT_EQ(i * i, o_a[i].y);
		ASSERT_EQ(i, o_a.at(i).x);
		ASSERT_EQ(i * i, o_a.at(i).y);
	}
}

TEST(containers_dynamic_array_test, copy_assign) {
	dynamic_array<dynamic_array_test_object_1> o_a(
			{{0, 0}, {1, 1}, {2, 4}, {3, 9}, {4, 16}, {5, 25}, {6, 36}});
	dynamic_array<dynamic_array_test_object_1> a(10, {123, 456});

	a = o_a;

	ASSERT_EQ(7, a.size());
	for(int i = 0; i < 7; ++i) {
		ASSERT_EQ(i, a[i].x);
		ASSERT_EQ(i * i, a[i].y);
		ASSERT_EQ(i, a.at(i).x);
		ASSERT_EQ(i * i, a.at(i).y);
	}
	ASSERT_EQ(7, o_a.size());
	for(int i = 0; i < 7; ++i) {
		ASSERT_EQ(i, o_a[i].x);
		ASSERT_EQ(i * i, o_a[i].y);
		ASSERT_EQ(i, o_a.at(i).x);
		ASSERT_EQ(i * i, o_a.at(i).y);
	}
}

TEST(containers_dynamic_array_test, move_construct) {
	dynamic_array<dynamic_array_test_object_1> o_a(
			{{0, 0}, {1, 1}, {2, 4}, {3, 9}, {4, 16}, {5, 25}, {6, 36}});

	auto a = std::move(o_a);

	ASSERT_EQ(7, a.size());
	for(int i = 0; i < 7; ++i) {
		ASSERT_EQ(i, a[i].x);
		ASSERT_EQ(i * i, a[i].y);
		ASSERT_EQ(i, a.at(i).x);
		ASSERT_EQ(i * i, a.at(i).y);
	}
	ASSERT_EQ(0, o_a.size());
	ASSERT_EQ(nullptr, o_a.data());
}

TEST(containers_dynamic_array_test, move_assign) {
	dynamic_array<dynamic_array_test_object_1> o_a(
			{{0, 0}, {1, 1}, {2, 4}, {3, 9}, {4, 16}, {5, 25}, {6, 36}});
	dynamic_array<dynamic_array_test_object_1> a(10, {123, 456});

	a = std::move(o_a);

	ASSERT_EQ(7, a.size());
	for(int i = 0; i < 7; ++i) {
		ASSERT_EQ(i, a[i].x);
		ASSERT_EQ(i * i, a[i].y);
		ASSERT_EQ(i, a.at(i).x);
		ASSERT_EQ(i * i, a.at(i).y);
	}
	ASSERT_EQ(0, o_a.size());
	ASSERT_EQ(nullptr, o_a.data());
}

struct dynamic_array_test_object_2 {
	struct A {
		int v;
	};
	struct B {
		int v;
	};
};

struct dynamic_array_test_object_2a {
	dynamic_array_test_object_2::A a;
	dynamic_array_test_object_2::B b;
};

struct dynamic_array_test_object_2b {
	dynamic_array_test_object_2::A a;
	dynamic_array_test_object_2::B b;
	dynamic_array_test_object_2b(dynamic_array_test_object_2::A a, dynamic_array_test_object_2::B b)
			: a{a}, b{b} {}
	dynamic_array_test_object_2b(std::initializer_list<dynamic_array_test_object_2::A>) {}
};

struct dynamic_array_test_object_2c {
	size_t a;
	int b;
	dynamic_array_test_object_2c(size_t, int) : a(1), b(2) {}
	dynamic_array_test_object_2c(std::initializer_list<int>) : a(0), b(0) {}
};

struct dynamic_array_test_object_2d {
	size_t a;
	int b;
};

TEST(containers_dynamic_array_test, construct_aggregate_init) {
	dynamic_array<dynamic_array_test_object_2a> da(10, dynamic_array_test_object_2::A{1},
												   dynamic_array_test_object_2::B{2});
	ASSERT_EQ(1, da[0].a.v);
	ASSERT_EQ(2, da[0].b.v);
}
TEST(containers_dynamic_array_test, construct_constructor_init) {
	dynamic_array<dynamic_array_test_object_2b> da(10, dynamic_array_test_object_2::A{1},
												   dynamic_array_test_object_2::B{2});
	ASSERT_EQ(1, da[0].a.v);
	ASSERT_EQ(2, da[0].b.v);
}
TEST(containers_dynamic_array_test, construct_constructor_init2) {
	dynamic_array<dynamic_array_test_object_2c> da(10, size_t(42), int(123));
	ASSERT_EQ(1, da[0].a);
	ASSERT_EQ(2, da[0].b);
	dynamic_array<dynamic_array_test_object_2c> da2(10, int(42), int(123), int(456));
	ASSERT_EQ(0, da2[0].a);
	ASSERT_EQ(0, da2[0].b);
}
TEST(containers_dynamic_array_test, construct_aggregate_init2) {
	dynamic_array<dynamic_array_test_object_2d> da(10, size_t(42), int(123));
	ASSERT_EQ(42, da[0].a);
	ASSERT_EQ(123, da[0].b);
}

} // namespace containers
} // namespace mce
