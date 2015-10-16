/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/unorderedobjectpool_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <containers/unordered_object_pool.hpp>
#include <boost/test/unit_test.hpp>
#include <unordered_set>
#include <string>
#include <algorithm>

namespace mce {
namespace containers {

struct unordered_object_pool_fixture {
	typedef long long element;
	mce::containers::unordered_object_pool<element,0x100u> uop;
	unordered_object_pool_fixture() {
		for(int i = 0; i < 1024; ++i) { uop.emplace(i); }
	}
	~unordered_object_pool_fixture() {
	}
	void checkSet(std::unordered_multiset<element> expected) {
		bool correct = true;
		for(auto& s : uop) {
			auto it = expected.find(s);
			if(it != expected.end()) {
				expected.erase(it);
			} else {
				correct = false;
			}
		}
		BOOST_CHECK(correct);
		BOOST_CHECK(expected.empty());
	}
	std::unordered_multiset<element> startSet() {
		std::unordered_multiset<element> s;
		for(int i = 0; i < 1024; ++i) { s.emplace(i); }
		return s;
	}
};

BOOST_AUTO_TEST_SUITE(containers)
BOOST_FIXTURE_TEST_SUITE(unordered_object_pool_test, unordered_object_pool_fixture)
BOOST_AUTO_TEST_CASE(emplace_one) {
	auto it = uop.emplace(42);
	BOOST_CHECK(*it == 42);
	auto expect = startSet();
	expect.emplace(42);
	checkSet(expect);
}
BOOST_AUTO_TEST_CASE(emplace_many) {
	bool correct=true;
	for(int i = 0; i < 0x1000; ++i) {
		auto it = uop.emplace(i);
		if(*it != i) correct=false;
	}
	BOOST_CHECK(correct);
	auto expect = startSet();
	for(int i = 0; i < 0x1000; ++i) { expect.emplace(i); }
	checkSet(expect);
}
BOOST_AUTO_TEST_CASE(insert_one) {
	auto it = uop.emplace(42);
	BOOST_CHECK(*it == 42);
	auto expect = startSet();
	expect.insert(element(42));
	checkSet(expect);
}
BOOST_AUTO_TEST_CASE(insert_many) {
	bool correct=true;
	for(int i = 0; i < 0x1000; ++i) {
		auto it = uop.insert(element(i));
		if(*it != i) correct=false;
	}
	BOOST_CHECK(correct);
	auto expect = startSet();
	for(int i = 0; i < 0x1000; ++i) { expect.emplace(i); }
	checkSet(expect);
}
BOOST_AUTO_TEST_CASE(erase_one) {
	auto val = *uop.begin();
	auto it = uop.erase(uop.begin());
	BOOST_CHECK(it == uop.begin());
	auto expect = startSet();
	expect.erase(val);
	checkSet(expect);
}
BOOST_AUTO_TEST_CASE(erase_many) {
	auto pred = [](auto& v){
		return v%2==0;
	};
	uop.erase(std::remove_if(uop.begin(),uop.end(),pred),uop.end());
	auto tmp = startSet();
	std::unordered_multiset<element> expect;
	for(auto& v:tmp){
		if(!pred(v)){
			expect.insert(v);
		}
	}
	checkSet(expect);
}
BOOST_AUTO_TEST_CASE(clear) {
	uop.clear();
	bool correct=true;
	for(auto& v:uop){
		(void)v;
		correct=false;
	}
	BOOST_CHECK(correct);
	BOOST_CHECK(uop.empty());
}

BOOST_AUTO_TEST_CASE(clear_and_refill) {
	uop.clear();
	bool correct=true;
	for(int i = 0; i < 0x2000; ++i) {
		auto it = uop.insert(element(i));
		if(*it != i) correct=false;
	}
	BOOST_CHECK(correct);
	std::unordered_multiset<element> expect;
	for(int i = 0; i < 0x2000; ++i) { expect.emplace(i); }
	checkSet(expect);
}
BOOST_AUTO_TEST_CASE(reorganize) {
	auto pred = [](auto& v){
		return v%2==0;
	};
	for(auto it=uop.begin();it!=uop.end();){
		if(pred(*it)){
			it=uop.erase(it);
		}
		else{
			++it;
		}
	}
	auto tmp = startSet();
	std::unordered_multiset<element> expect;
	for(auto& v:tmp){
		if(!pred(v)){
			expect.insert(v);
		}
	}
	size_t count = uop.reorganize();
	BOOST_CHECK(count!=0u);
	checkSet(expect);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} /* namespace containers */
} /* namespace mce */
