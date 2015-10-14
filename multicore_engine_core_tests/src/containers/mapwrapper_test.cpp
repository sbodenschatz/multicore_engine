/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/containers/mapwrapper_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <containers/map_wrapper.hpp>
#include <boost/test/unit_test.hpp>
#include <map>
#include <string>
#include <algorithm>
#include <future>
#include <boost/container/small_vector.hpp>
#include <iostream>

namespace mce {
namespace containers {

struct map_wrapper_fixture {
	typedef int key_element;
	typedef std::string value_element;
	template <typename T>
	using container = boost::container::small_vector<T, 32>;
	mce::containers::map_wrapper<container, key_element, value_element> mw;
	std::map<key_element, value_element> stdmap;
	map_wrapper_fixture() {}
	~map_wrapper_fixture() {}
};

BOOST_AUTO_TEST_SUITE(containers)
BOOST_FIXTURE_TEST_SUITE(map_wrapper_test, map_wrapper_fixture)

BOOST_AUTO_TEST_CASE(insert) {
	mw.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	mw.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	mw.insert(42, "hello");
	stdmap.emplace(42, "hello");
	mw.insert(43, "world");
	stdmap.emplace(43, "world");
	for(auto&& v : mw) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	for(auto& v : stdmap) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	BOOST_CHECK(std::equal(mw.begin(), mw.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
BOOST_AUTO_TEST_CASE(insert_duplicate) {
	mw.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	mw.insert(123, "EFG");
	stdmap.emplace(123, "EFG");
	mw.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	mw.insert(42, "hello");
	stdmap.emplace(42, "hello");
	mw.insert(43, "world");
	stdmap.emplace(43, "world");
	BOOST_CHECK(std::equal(mw.begin(), mw.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
BOOST_AUTO_TEST_CASE(insert_or_assign_duplicate) {
	mw.insert_or_assign(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	mw.insert_or_assign(123, "EFG");
	stdmap[123] = "EFG";
	mw.insert_or_assign(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	mw.insert_or_assign(42, "hello");
	stdmap.emplace(42, "hello");
	mw.insert_or_assign(43, "world");
	stdmap.emplace(43, "world");
	BOOST_CHECK(std::equal(mw.begin(), mw.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}

BOOST_AUTO_TEST_CASE(erase) {
	mw.insert(123, "ABCD");
	stdmap.emplace(123, "ABCD");
	mw.insert(456, "XYZ");
	stdmap.emplace(456, "XYZ");
	mw.insert(42, "hello");
	stdmap.emplace(42, "hello");
	mw.insert(43, "world");
	stdmap.emplace(43, "world");

	mw.erase(456);
	stdmap.erase(456);
	for(auto&& v : mw) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	for(auto& v : stdmap) { std::cout << v.first << " " << v.second << "   "; }
	std::cout << std::endl;
	BOOST_CHECK(std::equal(mw.begin(), mw.end(), stdmap.begin(), stdmap.end(), [](auto&& v1, auto&& v2) {
		return v1.first == v2.first && v1.second == v2.second;
	}));
}
BOOST_AUTO_TEST_CASE(dummy) {}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} /* namespace containers */
} /* namespace mce */
