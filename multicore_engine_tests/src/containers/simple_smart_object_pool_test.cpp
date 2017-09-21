/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/containers/simple_smart_object_pool_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <future>
#include <gtest.hpp>
#include <iterator>
#include <mce/containers/simple_smart_object_pool.hpp>
#include <mce/util/unused.hpp>
#include <string>
#include <unordered_set>

namespace mce {
namespace containers {

struct containers_simple_smart_object_pool_test : public ::testing::Test {
	struct X {
		long long x;
		explicit X(const long long& o) : x(o) {}
		X& operator=(const long long& o) {
			x = o;
			return *this;
		}
		X(const X& o) : x(o.x) {}
		X& operator=(const X& o) {
			x = o.x;
			return *this;
		}
		~X() {
			x = 0xdeadbeefdeadbeef;
		}
		operator long long() const {
			return x;
		}
	};
	typedef X element;
	mce::containers::simple_smart_object_pool<element> sop;
	containers_simple_smart_object_pool_test() {}
	~containers_simple_smart_object_pool_test() {}
};

TEST_F(containers_simple_smart_object_pool_test, emplace_and_destroy_one) {
	auto ptr = sop.emplace(42);
	ASSERT_TRUE(*ptr == 42);
	sop.process_pending();
	ASSERT_TRUE(sop.size() == 1);
	ptr.reset();
	sop.process_pending();
	ASSERT_TRUE(sop.size() == 0);
	ASSERT_TRUE(sop.empty());
}

TEST_F(containers_simple_smart_object_pool_test, emplace_and_destroy_many) {
	std::vector<std::shared_ptr<element>> elem_ptrs;
	for(int i = 0; i < 512; ++i) {
		auto ptr = sop.emplace(i);
		elem_ptrs.emplace_back(ptr);
		ASSERT_TRUE(*ptr == i);
	}
	sop.process_pending();
	ASSERT_TRUE(sop.size() == 512);
	for(int i = 0; i < 512; ++i) {
		ASSERT_TRUE(*(elem_ptrs[i]) == i);
	}
	elem_ptrs.clear();
	sop.process_pending();
	ASSERT_TRUE(sop.size() == 0);
}

TEST_F(containers_simple_smart_object_pool_test, emplace_and_iterate) {
	std::vector<std::shared_ptr<element>> elem_ptrs;
	std::vector<long long> vals_expected;
	for(int i = 0; i < 512; ++i) {
		auto ptr = sop.emplace(i);
		elem_ptrs.emplace_back(ptr);
		vals_expected.push_back(i);
		ASSERT_TRUE(*ptr == i);
	}
	sop.process_pending();
	std::vector<long long> vals;
	std::copy(sop.begin(), sop.end(), std::back_inserter(vals));
	std::sort(vals.begin(), vals.end());
	ASSERT_EQ(vals_expected, vals);
}

TEST_F(containers_simple_smart_object_pool_test, iterators) {
	auto ptr = sop.emplace(42);
	sop.process_pending();
	ASSERT_EQ(42, sop.begin()->x);
	auto it = sop.begin();
	long long x = *it;
	ASSERT_EQ(42, x);
	ASSERT_EQ(1, sop.end() - sop.begin());
	simple_smart_object_pool<element>::const_iterator const_it = it;
	simple_smart_object_pool<element>::const_iterator const_it2 = const_it;
	simple_smart_object_pool<element>::iterator it2 = it;
	// Must not compile:
	// simple_smart_object_pool<element>::iterator it3 = const_it;
	ASSERT_EQ(42, *const_it);
	ASSERT_EQ(42, *const_it2);
	ASSERT_EQ(42, *it2);
	ASSERT_EQ(42, sop.begin()[0]);
}

TEST_F(containers_simple_smart_object_pool_test, mt_emplace_and_destroy_many) {
	sop.reserve(128 * 0x1000);
	std::vector<std::future<bool>> futures;
	for(int t = 0; t < 256; ++t) {
		futures.emplace_back(std::async(std::launch::async,
										[&](int t) {
											bool res = true;
											std::vector<std::shared_ptr<element>> elem_ptrs;
											elem_ptrs.reserve(0x1000);
											for(int i = 0; i < 0x1000; ++i) {
												auto ptr = sop.emplace((t << 16) | i);
												elem_ptrs.emplace_back(ptr);
												res = res && (*ptr == ((t << 16) | i));
											}
											for(int i = 0; i < 0x1000; ++i) {
												res = res && (*(elem_ptrs[i]) == ((t << 16) | i));
											}
											elem_ptrs.clear();
											return res;
										},
										t));
	}
	sop.process_pending();
	for(auto& f : futures) {
		ASSERT_TRUE(f.get());
	}
}

} /* namespace containers */
} /* namespace mce */
