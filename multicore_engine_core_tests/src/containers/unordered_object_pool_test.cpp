/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/unorderedobjectpool_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <containers/unordered_object_pool.hpp>
#include <gtest.hpp>
#include <string>
#include <unordered_set>

namespace mce {
namespace containers {

struct containers_unordered_object_pool_test : public ::testing::Test {
	typedef long long element;
	mce::containers::unordered_object_pool<element, 0x100u> uop;
	containers_unordered_object_pool_test() {
		for(int i = 0; i < 1024; ++i) {
			uop.emplace(i);
		}
	}
	~containers_unordered_object_pool_test() {}
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
		ASSERT_TRUE(correct);
		ASSERT_TRUE(expected.empty());
	}
	std::unordered_multiset<element> startSet() {
		std::unordered_multiset<element> s;
		for(int i = 0; i < 1024; ++i) {
			s.emplace(i);
		}
		return s;
	}
};

TEST_F(containers_unordered_object_pool_test, emplace_one) {
	auto it = uop.emplace(42);
	ASSERT_TRUE(*it == 42);
	auto expect = startSet();
	expect.emplace(42);
	checkSet(expect);
}
TEST_F(containers_unordered_object_pool_test, emplace_many) {
	bool correct = true;
	for(int i = 0; i < 0x1000; ++i) {
		auto it = uop.emplace(i);
		if(*it != i) correct = false;
	}
	ASSERT_TRUE(correct);
	auto expect = startSet();
	for(int i = 0; i < 0x1000; ++i) {
		expect.emplace(i);
	}
	checkSet(expect);
}
TEST_F(containers_unordered_object_pool_test, insert_one) {
	auto it = uop.emplace(42);
	ASSERT_TRUE(*it == 42);
	auto expect = startSet();
	expect.insert(element(42));
	checkSet(expect);
}
TEST_F(containers_unordered_object_pool_test, insert_many) {
	bool correct = true;
	for(int i = 0; i < 0x1000; ++i) {
		auto it = uop.insert(element(i));
		if(*it != i) correct = false;
	}
	ASSERT_TRUE(correct);
	auto expect = startSet();
	for(int i = 0; i < 0x1000; ++i) {
		expect.emplace(i);
	}
	checkSet(expect);
}
TEST_F(containers_unordered_object_pool_test, erase_one) {
	auto val = *uop.begin();
	auto it = uop.erase(uop.begin());
	ASSERT_TRUE(it == uop.begin());
	auto expect = startSet();
	expect.erase(val);
	checkSet(expect);
}
TEST_F(containers_unordered_object_pool_test, erase_many) {
	auto pred = [](auto& v) { return v % 2 == 0; };
	uop.erase(std::remove_if(uop.begin(), uop.end(), pred), uop.end());
	auto tmp = startSet();
	std::unordered_multiset<element> expect;
	for(auto& v : tmp) {
		if(!pred(v)) {
			expect.insert(v);
		}
	}
	checkSet(expect);
}
TEST_F(containers_unordered_object_pool_test, clear) {
	uop.clear();
	bool correct = true;
	for(auto& v : uop) {
		(void)v;
		correct = false;
	}
	ASSERT_TRUE(correct);
	ASSERT_TRUE(uop.empty());
}

TEST_F(containers_unordered_object_pool_test, clear_and_refill) {
	uop.clear();
	bool correct = true;
	for(int i = 0; i < 0x2000; ++i) {
		auto it = uop.insert(element(i));
		if(*it != i) correct = false;
	}
	ASSERT_TRUE(correct);
	std::unordered_multiset<element> expect;
	for(int i = 0; i < 0x2000; ++i) {
		expect.emplace(i);
	}
	checkSet(expect);
}
TEST_F(containers_unordered_object_pool_test, reorganize) {
	auto pred = [](auto& v) { return v % 2 == 0; };
	for(auto it = uop.begin(); it != uop.end();) {
		if(pred(*it)) {
			it = uop.erase(it);
		} else {
			++it;
		}
	}
	auto tmp = startSet();
	std::unordered_multiset<element> expect;
	for(auto& v : tmp) {
		if(!pred(v)) {
			expect.insert(v);
		}
	}
	size_t count = uop.reorganize();
	ASSERT_TRUE(count != 0u);
	checkSet(expect);
}

} /* namespace containers */
} /* namespace mce */
