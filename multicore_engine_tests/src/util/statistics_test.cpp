/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/statistics_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <gtest.hpp>
#include <mce/util/statistics.hpp>

namespace mce {
namespace util {

TEST(util_statistics_test, aggregate_float) {
	aggregate_statistic<float> s;
	s.record(1.0f);
	s.record(2.0f);
	s.record(3.0f);
	s.record(10.0f);
	s.record(11.0f);
	s.record(12.0f);
	auto r = s.evaluate();
	ASSERT_FLOAT_EQ(r.average, 6.5f);
	ASSERT_FLOAT_EQ(r.sum, 39.0f);
	ASSERT_FLOAT_EQ(r.minimum, 1.0f);
	ASSERT_FLOAT_EQ(r.maximum, 12.0f);
	ASSERT_EQ(r.count, 6);
}

TEST(util_statistics_test, aggregate_int) {
	aggregate_statistic<int> s;
	s.record(1);
	s.record(2);
	s.record(3);
	s.record(10);
	s.record(11);
	s.record(12);
	auto r = s.evaluate<float>();
	ASSERT_FLOAT_EQ(r.average, 6.5f);
	ASSERT_EQ(r.sum, 39);
	ASSERT_EQ(r.minimum, 1);
	ASSERT_EQ(r.maximum, 12);
	ASSERT_EQ(r.count, 6);
}

} // namespace util
} // namespace mce
