/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/util/algorithm_test.cpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#include <algorithm>
#include <array>
#include <gtest.hpp>
#include <mce/util/algorithm.hpp>
#include <vector>

namespace mce {
namespace util {

TEST(util_algorithm, merge_adjacent_if_simple_numbers) {
	std::vector<int> data = {2, 5, 4, 2, 3, 5, 8, 2, 4, 5, 3};
	std::sort(data.begin(), data.end());
	auto it = merge_adjacent_if(data.begin(), data.end(), [](auto& a, auto& b) { return a == b; },
								[](auto& a, auto& b) { ASSERT_TRUE(a == b); });
	data.erase(it, data.end());
	std::vector<int> data_expected = {2, 3, 4, 5, 8};
	ASSERT_TRUE(data == data_expected);
}

TEST(util_algorithm, merge_adjacent_if_ranges_multiple) {
	std::vector<std::pair<int, int>> data = {{0, 10},  {10, 20}, {20, 30}, {30, 40}, {40, 50},
											 {60, 70}, {70, 80}, {80, 90}, {90, 100}};
	auto it =
			merge_adjacent_if(data.begin(), data.end(), [](auto& a, auto& b) { return a.second == b.first; },
							  [](auto& a, auto& b) {
								  ASSERT_TRUE(a.second == b.first);
								  a.second = b.second;
							  });
	data.erase(it, data.end());
	std::vector<std::pair<int, int>> data_expected = {{0, 50}, {60, 100}};
	ASSERT_TRUE(data == data_expected);
}

TEST(util_algorithm, merge_adjacent_if_ranges_single) {
	std::vector<std::pair<int, int>> data = {{0, 10},  {10, 20}, {20, 30}, {30, 40}, {40, 50},
											 {50, 60}, {60, 70}, {70, 80}, {80, 90}, {90, 100}};
	auto it =
			merge_adjacent_if(data.begin(), data.end(), [](auto& a, auto& b) { return a.second == b.first; },
							  [](auto& a, auto& b) {
								  ASSERT_TRUE(a.second == b.first);
								  a.second = b.second;
							  });
	data.erase(it, data.end());
	std::vector<std::pair<int, int>> data_expected = {{0, 100}};
	ASSERT_TRUE(data == data_expected);
}

TEST(util_algorithm, merge_adjacent_if_ranges_no_merge) {
	std::vector<std::pair<int, int>> data = {{0, 9},   {10, 19}, {20, 29}, {30, 39}, {40, 49},
											 {50, 59}, {60, 69}, {70, 79}, {80, 89}, {90, 99}};
	auto it =
			merge_adjacent_if(data.begin(), data.end(), [](auto& a, auto& b) { return a.second == b.first; },
							  [](auto& a, auto& b) {
								  ASSERT_TRUE(a.second == b.first);
								  a.second = b.second;
							  });
	data.erase(it, data.end());
	std::vector<std::pair<int, int>> data_expected = {{0, 9},   {10, 19}, {20, 29}, {30, 39}, {40, 49},
													  {50, 59}, {60, 69}, {70, 79}, {80, 89}, {90, 99}};
	ASSERT_TRUE(data == data_expected);
}

TEST(util_algorithm, merge_adjacent_if_ranges_empty) {
	std::vector<std::pair<int, int>> data = {};
	auto it =
			merge_adjacent_if(data.begin(), data.end(), [](auto& a, auto& b) { return a.second == b.first; },
							  [](auto& a, auto& b) {
								  ASSERT_TRUE(a.second == b.first);
								  a.second = b.second;
							  });
	data.erase(it, data.end());
	std::vector<std::pair<int, int>> data_expected = {};
	ASSERT_TRUE(data == data_expected);
}

TEST(util_algorithm, merge_adjacent_if_ranges_1_element) {
	std::vector<std::pair<int, int>> data = {{0, 99}};
	auto it =
			merge_adjacent_if(data.begin(), data.end(), [](auto& a, auto& b) { return a.second == b.first; },
							  [](auto& a, auto& b) {
								  ASSERT_TRUE(a.second == b.first);
								  a.second = b.second;
							  });
	data.erase(it, data.end());
	std::vector<std::pair<int, int>> data_expected = {{0, 99}};
	ASSERT_TRUE(data == data_expected);
}

TEST(util_algorithm, n_unique_full_groups) {
	int groups = 10;
	int elements = 20;
	std::vector<int> data;
	data.reserve(groups * elements);
	std::vector<int> data_expected;
	data_expected.reserve(groups * elements);
	for(int n = 0; n < 20; ++n) {
		data.clear();
		data_expected.clear();
		for(int i = 0; i < groups; ++i) {
			for(int j = 0; j < elements; ++j) {
				data.push_back(i);
			}
		}
		auto new_end = n_unique(data.begin(), data.end(), std::equal_to<>(), n);
		data.erase(new_end, data.end());
		for(int i = 0; i < groups; ++i) {
			for(int j = 0; j < n; ++j) {
				data_expected.push_back(i);
			}
		}
		ASSERT_TRUE(data == data_expected);
	}
}

TEST(util_algorithm, n_unique_partial_groups) {
	int groups = 10;
	int elements = 5;
	std::vector<int> data;
	data.reserve(groups * elements);
	std::vector<int> data_expected;
	data_expected.reserve(groups * elements);
	for(int n = 10; n < 20; ++n) {
		data.clear();
		data_expected.clear();
		for(int i = 0; i < groups; ++i) {
			for(int j = 0; j < elements; ++j) {
				data.push_back(i);
			}
		}
		auto new_end = n_unique(data.begin(), data.end(), std::equal_to<>(), n);
		data.erase(new_end, data.end());
		for(int i = 0; i < groups; ++i) {
			for(int j = 0; j < elements; ++j) {
				data_expected.push_back(i);
			}
		}
		ASSERT_TRUE(data == data_expected);
	}
}

TEST(util_algorithm, grouped_foreach_1_grouping) {
	using element = std::array<int, 3>;
	std::string res;
	std::vector<element> elements = {{{0, 0, 0}}, {{0, 0, 1}}, {{0, 0, 2}}, {{2, 0, 0}}, {{2, 1, 0}},
									 {{2, 1, 1}}, {{3, 0, 0}}, {{3, 0, 1}}, {{3, 1, 0}}, {{3, 1, 1}}};
	grouped_foreach(elements.begin(), elements.end(),
					[&res](const element& e) {
						res += "(" + std::to_string(e[0]) + " " + std::to_string(e[1]) + " " +
							   std::to_string(e[2]) + ")";
					},
					make_foreach_grouping([](const element& e1, const element& e2) { return e1[0] == e2[0]; },
										  [&res](const element&) { res += "A["; },
										  [&res](const element&) { res += "]A"; }));
	ASSERT_EQ("A[(0 0 0)(0 0 1)(0 0 2)]AA[(2 0 0)(2 1 0)(2 1 1)]AA[(3 0 0)(3 0 1)(3 1 0)(3 1 1)]A", res);
}

TEST(util_algorithm, grouped_foreach_2_groupings) {
	using element = std::array<int, 3>;
	std::string res;
	std::vector<element> elements = {{{0, 0, 0}}, {{0, 0, 1}}, {{0, 0, 2}}, {{2, 0, 0}}, {{2, 1, 0}},
									 {{2, 1, 1}}, {{3, 0, 0}}, {{3, 0, 1}}, {{3, 1, 0}}, {{3, 1, 1}}};
	grouped_foreach(elements.begin(), elements.end(),
					[&res](const element& e) {
						res += "(" + std::to_string(e[0]) + " " + std::to_string(e[1]) + " " +
							   std::to_string(e[2]) + ")";
					},
					make_foreach_grouping([](const element& e1, const element& e2) { return e1[0] == e2[0]; },
										  [&res](const element&) { res += "A["; },
										  [&res](const element&) { res += "]A"; }),
					make_foreach_grouping([](const element& e1, const element& e2) { return e1[1] == e2[1]; },
										  [&res](const element&) { res += "B["; },
										  [&res](const element&) { res += "]B"; }));
	ASSERT_EQ("A[B[(0 0 0)(0 0 1)(0 0 2)]B]AA[B[(2 0 0)]BB[(2 1 0)(2 1 1)]B]A"
			  "A[B[(3 0 0)(3 0 1)]BB[(3 1 0)(3 1 1)]B]A",
			  res);
}

} // namespace util
} // namespace mce
