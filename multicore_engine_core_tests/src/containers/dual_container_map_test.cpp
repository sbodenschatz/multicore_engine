/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/containers/mapwrapper_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <map>
#include <string>
#include <algorithm>
#include <future>
#include <boost/container/small_vector.hpp>
#include <containers/dual_container_map.hpp>
#include <iostream>

namespace mce {
namespace containers {

struct dual_container_map_fixture {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = boost::container::small_vector<T, 32>;
	mce::containers::dual_container_map<container, key_element, value_element> dcm;
	std::map<key_element, value_element> stdmap;
	dual_container_map_fixture() {}
	~dual_container_map_fixture() {}
};

struct dual_container_multimap_fixture {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = boost::container::small_vector<T, 32>;
	mce::containers::dual_container_multimap<container, key_element, value_element> dcm;
	std::multimap<key_element, value_element> stdmap;
	dual_container_multimap_fixture() {}
	~dual_container_multimap_fixture() {}
};

BOOST_AUTO_TEST_SUITE(containers)
BOOST_FIXTURE_TEST_SUITE(dual_container_map_test, dual_container_map_fixture)

BOOST_AUTO_TEST_CASE(insert) {
	dcm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	dcm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	dcm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	dcm.insert(43, "world");
	stdmap.emplace(43, "world");
	BOOST_CHECK(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
BOOST_AUTO_TEST_CASE(insert_duplicate) {
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
	BOOST_CHECK(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
BOOST_AUTO_TEST_CASE(insert_or_assign_duplicate) {
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
	BOOST_CHECK(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

BOOST_AUTO_TEST_CASE(erase) {
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
	BOOST_CHECK(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(dual_container_multimap_test, dual_container_multimap_fixture)

BOOST_AUTO_TEST_CASE(insert) {
	dcm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	dcm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	dcm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	dcm.insert(43, "world");
	stdmap.emplace(43, "world");
	BOOST_CHECK(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
BOOST_AUTO_TEST_CASE(insert_duplicate) {
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
	BOOST_CHECK(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

BOOST_AUTO_TEST_CASE(erase) {
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
	BOOST_CHECK(std::equal(dcm.begin(), dcm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

} /* namespace containers */
} /* namespace mce */
