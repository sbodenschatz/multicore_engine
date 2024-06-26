/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/containers/generic_flat_map_test.cpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <boost/container/small_vector.hpp>
#include <future>
#include <gtest.hpp>
#include <iostream>
#include <map>
#include <mce/containers/generic_flat_map.hpp>
#include <string>
#include <vector>

namespace mce {
namespace containers {

struct containers_generic_flat_map_test : public ::testing::Test {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = boost::container::small_vector<T, 32>;
	mce::containers::generic_flat_map<container, key_element, value_element> gfm;
	std::map<key_element, value_element> stdmap;
	containers_generic_flat_map_test() {}
	~containers_generic_flat_map_test() {}
};

struct containers_generic_flat_multimap_test : public ::testing::Test {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = boost::container::small_vector<T, 32>;
	mce::containers::generic_flat_multimap<container, key_element, value_element> gfm;
	std::multimap<key_element, value_element> stdmap;
	containers_generic_flat_multimap_test() {}
	~containers_generic_flat_multimap_test() {}
};

struct containers_generic_flat_map_stdvec_test : public ::testing::Test {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = std::vector<T>;
	mce::containers::generic_flat_map<container, key_element, value_element> gfm;
	std::map<key_element, value_element> stdmap;
	containers_generic_flat_map_stdvec_test() {}
	~containers_generic_flat_map_stdvec_test() {}
};

struct containers_generic_flat_multimap_stdvec_test : public ::testing::Test {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = std::vector<T>;
	mce::containers::generic_flat_multimap<container, key_element, value_element> gfm;
	std::multimap<key_element, value_element> stdmap;
	containers_generic_flat_multimap_stdvec_test() {}
	~containers_generic_flat_multimap_stdvec_test() {}
};

// containers_generic_flat_map_test

TEST_F(containers_generic_flat_map_test, insert) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
TEST_F(containers_generic_flat_map_test, insert_duplicate) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(123, "EFG");
	stdmap.emplace(123, "EFG");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
TEST_F(containers_generic_flat_map_test, insert_or_assign_duplicate) {
	gfm.insert_or_assign(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert_or_assign(123, "EFG");
	stdmap[123] = "EFG";
	gfm.insert_or_assign(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert_or_assign(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert_or_assign(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

TEST_F(containers_generic_flat_map_test, erase) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");

	gfm.erase(456);
	stdmap.erase(456);
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

// containers_generic_flat_multimap_test

TEST_F(containers_generic_flat_multimap_test, insert) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
TEST_F(containers_generic_flat_multimap_test, insert_duplicate) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(123, "EFG");
	stdmap.emplace(123, "EFG");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

TEST_F(containers_generic_flat_multimap_test, erase) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");

	gfm.erase(456);
	stdmap.erase(456);
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

// containers_generic_flat_map_stdvec_test

TEST_F(containers_generic_flat_map_stdvec_test, insert) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
TEST_F(containers_generic_flat_map_stdvec_test, insert_duplicate) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(123, "EFG");
	stdmap.emplace(123, "EFG");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
TEST_F(containers_generic_flat_map_stdvec_test, insert_or_assign_duplicate) {
	gfm.insert_or_assign(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert_or_assign(123, "EFG");
	stdmap[123] = "EFG";
	gfm.insert_or_assign(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert_or_assign(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert_or_assign(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

TEST_F(containers_generic_flat_map_stdvec_test, erase) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");

	gfm.erase(456);
	stdmap.erase(456);
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

// containers_generic_flat_multimap_stdvec_test

TEST_F(containers_generic_flat_multimap_stdvec_test, insert) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
TEST_F(containers_generic_flat_multimap_stdvec_test, insert_duplicate) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(123, "EFG");
	stdmap.emplace(123, "EFG");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

TEST_F(containers_generic_flat_multimap_stdvec_test, erase) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");

	gfm.erase(456);
	stdmap.erase(456);
	ASSERT_TRUE(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

} /* namespace containers */
} /* namespace mce */
