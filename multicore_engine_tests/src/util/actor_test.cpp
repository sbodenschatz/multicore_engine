/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/actor_test.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <atomic>
#include <gtest.hpp>
#include <mce/util/actor.hpp>

namespace mce {
namespace util {

struct actor_test_object {
	static uint64_t step_id() {
		static std::atomic<uint64_t> val{0};
		return ++val;
	}
	uint64_t a_ = 0;
	uint64_t b_ = 0;
	uint64_t c_ = 0;
	uint64_t d_ = 0;
	uint64_t e1_ = 0;
	uint64_t e2_ = 0;

	void a() {
		a_ = step_id();
	}
	void b() {
		b_ = step_id();
	}
	void c() {
		c_ = step_id();
	}
	void d(uint64_t v) {
		d_ = v;
	}
	void e(uint64_t v1, uint64_t v2) {
		e1_ = v1;
		e2_ = v2;
	}
};

TEST(util_actor_test, enqueue_lambda) {
	actor_test_object obj;
	actor<actor_test_object, 0x100> actr(&obj);
	actr.enqueue([](actor_test_object* o) { o->a(); });
	actr.enqueue([](actor_test_object* o) { o->b(); });
	actr.enqueue([](actor_test_object* o) { o->c(); });
	actr.enqueue([](actor_test_object* o) { o->d(42); });
	actr.enqueue([](actor_test_object* o) { o->e(123, 456); });
	actr.process();

	ASSERT_TRUE(0 < obj.a_);
	ASSERT_TRUE(obj.a_ < obj.b_);
	ASSERT_TRUE(obj.b_ < obj.c_);
	ASSERT_TRUE(obj.d_ == 42);
	ASSERT_TRUE(obj.e1_ == 123);
	ASSERT_TRUE(obj.e2_ == 456);
}
TEST(util_actor_test, enqueue_member) {
	actor_test_object obj;
	actor<actor_test_object, 0x100> actr(&obj);
	actr.enqueue(&actor_test_object::a);
	actr.enqueue(&actor_test_object::b);
	actr.enqueue(&actor_test_object::c);
	actr.enqueue(&actor_test_object::d, 42);
	actr.enqueue(&actor_test_object::e, 123, 456);
	actr.process();

	ASSERT_TRUE(0 < obj.a_);
	ASSERT_TRUE(obj.a_ < obj.b_);
	ASSERT_TRUE(obj.b_ < obj.c_);
	ASSERT_TRUE(obj.d_ == 42);
	ASSERT_TRUE(obj.e1_ == 123);
	ASSERT_TRUE(obj.e2_ == 456);
}

} // namespace util
} // namespace mce
