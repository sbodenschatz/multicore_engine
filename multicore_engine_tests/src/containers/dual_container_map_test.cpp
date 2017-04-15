/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/containers/mapwrapper_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/container/small_vector.hpp>
#include <containers/dual_container_map.hpp>
#include <future>
#include <gtest.hpp>
#include <iostream>
#include <map>
#include <string>

namespace mce {
namespace containers {

struct containers_dual_container_map_test : public ::testing::Test {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = boost::container::small_vector<T, 32>;
	mce::containers::dual_container_map<container, key_element, value_element> dcm;
	std::map<key_element, value_element> stdmap;
	containers_dual_container_map_test() {}
	~containers_dual_container_map_test() {}
};

struct containers_dual_container_multimap_test : public ::testing::Test {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = boost::container::small_vector<T, 32>;
	mce::containers::dual_container_multimap<container, key_element, value_element> dcm;
	std::multimap<key_element, value_element> stdmap;
	containers_dual_container_multimap_test() {}
	~containers_dual_container_multimap_test() {}
};

// containers_dual_container_map_test

TEST_F(containers_dual_container_map_test, insert) {
	dcm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	dcm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	dcm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	dcm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
TEST_F(containers_dual_container_map_test, insert_duplicate) {
	dcm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	dcm.insert(123, "EFG");
	stdmap.emplace(123, "EFG");
	dcm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	dcm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	dcm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
TEST_F(containers_dual_container_map_test, insert_or_assign_duplicate) {
	dcm.insert_or_assign(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	dcm.insert_or_assign(123, "EFG");
	stdmap[123] = "EFG";
	dcm.insert_or_assign(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	dcm.insert_or_assign(42, "hello");
	stdmap.emplace(42, "hello");
	dcm.insert_or_assign(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

TEST_F(containers_dual_container_map_test, erase) {
	dcm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	dcm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	dcm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	dcm.insert(43, "world");
	stdmap.emplace(43, "world");

	dcm.erase(456);
	stdmap.erase(456);
	ASSERT_TRUE(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

// containers_dual_container_multimap_test

TEST_F(containers_dual_container_multimap_test, insert) {
	dcm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	dcm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	dcm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	dcm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
TEST_F(containers_dual_container_multimap_test, insert_duplicate) {
	dcm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	dcm.insert(123, "EFG");
	stdmap.emplace(123, "EFG");
	dcm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	dcm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	dcm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

TEST_F(containers_dual_container_multimap_test, erase) {
	dcm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	dcm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	dcm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	dcm.insert(43, "world");
	stdmap.emplace(43, "world");

	dcm.erase(456);
	stdmap.erase(456);
	ASSERT_TRUE(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

} /* namespace containers */
} /* namespace mce */
