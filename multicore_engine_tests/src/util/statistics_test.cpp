/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/statistics_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <cmath>
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

TEST(util_statistics_test, histogram_bucket_bounds_int) {
	auto lower = 10;
	auto upper = 110;
	auto bucket_count = size_t(64);
	for(size_t i = 0; i <= bucket_count; ++i) {
		auto idx = detail::histogram_bucket_index(
				detail::histogram_bucket_lower_bound(i, lower, upper, bucket_count), lower, upper,
				bucket_count);
		ASSERT_TRUE(i == idx || i == idx + 1);
	}
}

TEST(util_statistics_test, histogram_bucket_mapping_float) {
	auto lower = 7.0f;
	auto upper = 263.0f;
	auto bucket_count = size_t(137);
	for(size_t i = 0; i <= bucket_count; ++i) {
		auto idx = detail::histogram_bucket_index(
				detail::histogram_bucket_lower_bound(i, lower, upper, bucket_count), lower, upper,
				bucket_count);
		ASSERT_TRUE(i == idx || i == idx + 1);
	}
}

TEST(util_statistics_test, histogram_int) {
	auto upper = 4096;
	auto count = 1024;
	histogram_statistic<int> s(0, upper, count);
	size_t total = 0;
	for(int i = 0; i < upper; ++i) {
		for(int j = 0; j < i; ++j) {
			s.record(i);
			++total;
		}
	}
	auto r = s.evaluate();
	ASSERT_EQ(0, r.under_samples);
	ASSERT_EQ(0, r.over_samples);
	ASSERT_EQ(total, r.total_samples);
	for(int i = 0; i < count; ++i) {
		ASSERT_EQ(i * upper / count, r.buckets.at(i).lower_bound);
		ASSERT_EQ((i + 1) * upper / count, r.buckets.at(i).upper_bound);
		ASSERT_EQ(4 * 4 * i + 1 + 2 + 3, r.buckets.at(i).samples);
	}
}

TEST(util_statistics_test, histogram_float) {
	auto upper = 4096.0f;
	auto count = 1024;
	histogram_statistic<float> s(0.0f, upper, count);
	size_t total = 0;
	for(float i = 0; i < upper; i += 1.0f) {
		for(int j = 0; j < i; ++j) {
			s.record(i);
			++total;
		}
	}
	auto r = s.evaluate();
	ASSERT_EQ(0, r.under_samples);
	ASSERT_EQ(0, r.over_samples);
	ASSERT_EQ(total, r.total_samples);
	for(int i = 0; i < count; ++i) {
		ASSERT_EQ(i * upper / count, r.buckets.at(i).lower_bound);
		ASSERT_EQ((i + 1) * upper / count, r.buckets.at(i).upper_bound);
		ASSERT_EQ(4 * 4 * i + 1 + 2 + 3, r.buckets.at(i).samples);
	}
}

} // namespace util
} // namespace mce
