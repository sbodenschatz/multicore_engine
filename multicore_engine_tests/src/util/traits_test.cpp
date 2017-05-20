/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/traits_test.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <glm/glm.hpp>
#include <gtest.hpp>
#include <string>
#include <type_traits>
#include <util/traits.hpp>
#include <vector>

namespace mce {
namespace util {

TEST(util_traits_test, swappable_positive) {
	struct A {
		A(A&&) {}
		A& operator=(A&&) {
			return *this;
		}
	};
	static_assert(is_swappable<A>::value, "is_swappable<A>::value");
	ASSERT_TRUE(is_swappable<A>::value);
}

/*
// TODO Fix on msvc
TEST(util_traits_test,swappable_negative) {
	struct A {
		A() {}
		A(A&&) = delete;
		A& operator=(A&&) = delete;
		A(const A&) = delete;
		A& operator=(const A&) = delete;
	};
	A a1, a2;
	using std::swap;
	// swap(a1, a2);
	// static_assert(!is_swappable<A>::value, "!is_swappable<A>::value");
	ASSERT_TRUE(!is_swappable<A>::value);
}
*/

TEST(util_traits_test, nothrow_swappable_positive) {
	struct A {
		A(A&&) noexcept {}
		A& operator=(A&&) noexcept {
			return *this;
		}
	};
	ASSERT_TRUE(is_nothrow_swappable<A>::value);
}

TEST(util_traits_test, nothrow_swappable_negative) {
	struct A {
		A(A&&) {}
		A& operator=(A&&) {
			return *this;
		}
	};
	ASSERT_TRUE(!is_nothrow_swappable<A>::value);
}

TEST(util_traits_test, accessor_value_type_test) {
	ASSERT_TRUE((std::is_same<util::accessor_value_type_t<int>, int>::value));
	ASSERT_TRUE((std::is_same<util::accessor_value_type_t<bool>, bool>::value));
	ASSERT_TRUE((std::is_same<util::accessor_value_type_t<float>, float>::value));
	ASSERT_TRUE((std::is_same<util::accessor_value_type_t<std::string>, const std::string&>::value));
	ASSERT_TRUE((std::is_same<util::accessor_value_type_t<glm::vec2>, const glm::vec2&>::value));
	ASSERT_TRUE(
			(std::is_same<util::accessor_value_type_t<std::vector<int>>, const std::vector<int>&>::value));
}

} // namespace util
} // namespace mce
