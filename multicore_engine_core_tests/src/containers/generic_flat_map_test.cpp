/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/containers/generic_flat_map_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <boost/test/unit_test.hpp>
#include <map>
#include <string>
#include <algorithm>
#include <future>
#include <boost/container/small_vector.hpp>
#include <containers/generic_flat_map.hpp>
#include <iostream>

namespace mce {
namespace containers {

struct generic_flat_map_fixture {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = boost::container::small_vector<T, 32>;
	mce::containers::generic_flat_map<container, key_element, value_element> gfm;
	std::map<key_element, value_element> stdmap;
	generic_flat_map_fixture() {}
	~generic_flat_map_fixture() {}
};

struct generic_flat_multimap_fixture {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = boost::container::small_vector<T, 32>;
	mce::containers::generic_flat_multimap<container, key_element, value_element> gfm;
	std::multimap<key_element, value_element> stdmap;
	generic_flat_multimap_fixture() {}
	~generic_flat_multimap_fixture() {}
};

BOOST_AUTO_TEST_SUITE(containers)
BOOST_FIXTURE_TEST_SUITE(generic_flat_map_test, generic_flat_map_fixture)

BOOST_AUTO_TEST_CASE(insert) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	for(auto&& v : gfm) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	for(auto& v : stdmap) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	BOOST_CHECK(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
BOOST_AUTO_TEST_CASE(insert_duplicate) {
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
	BOOST_CHECK(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
BOOST_AUTO_TEST_CASE(insert_or_assign_duplicate) {
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
	BOOST_CHECK(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

BOOST_AUTO_TEST_CASE(erase) {
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
	for(auto&& v : gfm) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	for(auto& v : stdmap) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	BOOST_CHECK(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(generic_flat_multimap_test, generic_flat_multimap_fixture)

BOOST_AUTO_TEST_CASE(insert) {
	gfm.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	gfm.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	gfm.insert(42, "hello");
	stdmap.emplace(42, "hello");
	gfm.insert(43, "world");
	stdmap.emplace(43, "world");
	for(auto&& v : gfm) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	for(auto& v : stdmap) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	BOOST_CHECK(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
BOOST_AUTO_TEST_CASE(insert_duplicate) {
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
	BOOST_CHECK(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

BOOST_AUTO_TEST_CASE(erase) {
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
	for(auto&& v : gfm) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	for(auto& v : stdmap) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	BOOST_CHECK(std::equal(gfm.begin(), gfm.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

} /* namespace containers */
} /* namespace mce */
