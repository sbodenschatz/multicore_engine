/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/util/traits_test.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <util/traits.hpp>

namespace mce {
namespace util {

BOOST_AUTO_TEST_SUITE(util)
BOOST_AUTO_TEST_SUITE(monitor_test)

BOOST_AUTO_TEST_CASE(swappable_positive) {
	struct A {
		A(A&&) {}
		A& operator=(A&&) {
			return *this;
		}
	};
	BOOST_CHECK(is_swappable<A>::value);
}

BOOST_AUTO_TEST_CASE(swappable_negative) {
	struct A {
		A(A&&) = delete;
		A& operator=(A&&) = delete;
		A(const A&) = delete;
		A& operator=(const A&) = delete;
	};
	BOOST_CHECK(!is_swappable<A>::value);
}

BOOST_AUTO_TEST_CASE(nothrow_swappable_positive) {
	struct A {
		A(A&&) noexcept {}
		A& operator=(A&&) noexcept {
			return *this;
		}
	};
	BOOST_CHECK(is_nothrow_swappable<A>::value);
}

BOOST_AUTO_TEST_CASE(nothrow_swappable_negative) {
	struct A {
		A(A&&) {}
		A& operator=(A&&) {
			return *this;
		}
	};
	BOOST_CHECK(!is_nothrow_swappable<A>::value);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace util
} // namespace mce
