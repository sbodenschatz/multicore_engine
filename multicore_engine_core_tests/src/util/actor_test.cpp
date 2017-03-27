/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/util/actor_test.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <atomic>
#include <boost/test/unit_test.hpp>
#include <util/actor.hpp>

namespace mce {
namespace util {

BOOST_AUTO_TEST_SUITE(util)
BOOST_AUTO_TEST_SUITE(actor_test)

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

BOOST_AUTO_TEST_CASE(enqueue_lambda) {
	actor_test_object obj;
	actor<actor_test_object, 0x100> actr(&obj);
	actr.enqueue([](actor_test_object* o) { o->a(); });
	actr.enqueue([](actor_test_object* o) { o->b(); });
	actr.enqueue([](actor_test_object* o) { o->c(); });
	actr.enqueue([](actor_test_object* o) { o->d(42); });
	actr.enqueue([](actor_test_object* o) { o->e(123, 456); });
	actr.process();

	BOOST_CHECK(0 < obj.a_);
	BOOST_CHECK(obj.a_ < obj.b_);
	BOOST_CHECK(obj.b_ < obj.c_);
	BOOST_CHECK(obj.d_ == 42);
	BOOST_CHECK(obj.e1_ == 123);
	BOOST_CHECK(obj.e2_ == 456);
}
BOOST_AUTO_TEST_CASE(enqueue_member) {
	actor_test_object obj;
	actor<actor_test_object, 0x100> actr(&obj);
	actr.enqueue(&actor_test_object::a);
	actr.enqueue(&actor_test_object::b);
	actr.enqueue(&actor_test_object::c);
	actr.enqueue(&actor_test_object::d, 42);
	actr.enqueue(&actor_test_object::e, 123, 456);
	actr.process();

	BOOST_CHECK(0 < obj.a_);
	BOOST_CHECK(obj.a_ < obj.b_);
	BOOST_CHECK(obj.b_ < obj.c_);
	BOOST_CHECK(obj.d_ == 42);
	BOOST_CHECK(obj.e1_ == 123);
	BOOST_CHECK(obj.e2_ == 456);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace util
} // namespace mce
