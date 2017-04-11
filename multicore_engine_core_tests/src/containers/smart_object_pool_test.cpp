/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/containers/smartobjectpool_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <algorithm>
#include <containers/smart_object_pool.hpp>
#include <future>
#include <gtest.hpp>
#include <iterator>
#include <string>
#include <unordered_set>
#include <util/unused.hpp>

namespace mce {
namespace containers {

struct containers_smart_object_pool_test : public ::testing::Test {
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
	mce::containers::smart_object_pool<element> sop;
	containers_smart_object_pool_test() {}
	~containers_smart_object_pool_test() {}
};

TEST_F(containers_smart_object_pool_test, emplace_and_destroy_one) {
	auto ptr = sop.emplace(42);
	ASSERT_TRUE(*ptr == 42);
	ASSERT_TRUE(sop.size() == 1);
	ptr.reset();
	ASSERT_TRUE(sop.size() == 0);
	ASSERT_TRUE(sop.empty());
}

TEST_F(containers_smart_object_pool_test, emplace_and_destroy_many) {
	std::vector<smart_pool_ptr<element>> elem_ptrs;
	for(int i = 0; i < 512; ++i) {
		auto ptr = sop.emplace(i);
		elem_ptrs.emplace_back(ptr);
		ASSERT_TRUE(*ptr == i);
	}
	ASSERT_TRUE(sop.size() == 512);
	for(int i = 0; i < 512; ++i) {
		ASSERT_TRUE(*(elem_ptrs[i]) == i);
	}
	elem_ptrs.clear();
	ASSERT_TRUE(sop.size() == 0);
}

TEST_F(containers_smart_object_pool_test, iterator_holds_object) {
	auto ptr = sop.emplace(42);
	ASSERT_TRUE(*ptr == 42);
	ASSERT_TRUE(sop.size() == 1);
	auto it = sop.begin();
	ptr.reset();
	ASSERT_TRUE(sop.size() == 1);
	it = decltype(sop)::iterator();
	ASSERT_TRUE(sop.size() == 0);
	ASSERT_TRUE(sop.empty());
}

TEST_F(containers_smart_object_pool_test, rescue_object) {
	auto ptr = sop.emplace(42);
	ASSERT_TRUE(*ptr == 42);
	ASSERT_TRUE(sop.size() == 1);
	auto it = sop.begin();
	weak_pool_ptr<element> wptr = ptr;
	ptr.reset();
	ptr = wptr.lock();
	ASSERT_TRUE(ptr);
	ASSERT_TRUE(sop.size() == 1);
	it = decltype(sop)::iterator();
	ASSERT_TRUE(*ptr == 42);
	ptr.reset();
	ASSERT_TRUE(sop.size() == 0);
	ASSERT_TRUE(sop.empty());
}

TEST_F(containers_smart_object_pool_test, mt_emplace_and_destroy_many) {
	auto t1 = std::chrono::high_resolution_clock::now();
	sop.reserve(128 * 0x1000);
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff1 = t2 - t1;
	// std::cout << diff1.count() << std::endl;
	UNUSED(diff1);
	std::vector<std::future<bool>> futures;
	for(int t = 0; t < 256; ++t) {
		futures.emplace_back(std::async(std::launch::async,
										[&](int t) {
											bool res = true;
											std::vector<smart_pool_ptr<element>> elem_ptrs;
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
	for(auto& f : futures) {
		ASSERT_TRUE(f.get());
	}
	auto t3 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff2 = t3 - t2;
	// std::cout << diff2.count() << std::endl;
	UNUSED(diff2);
}

TEST_F(containers_smart_object_pool_test, iterator_prevent_skip_over_end) {
	auto ptr1 = sop.emplace(1);
	auto ptr2 = sop.emplace(2);
	auto ptr3 = sop.emplace(3);
	auto ptr4 = sop.emplace(4);
	auto ptr5 = sop.emplace(5);

	auto it1 = sop.begin();
	std::advance(it1, 3);
	it1 = it1.make_limiter();
	auto it2 = sop.begin();
	long long val = 0;
	for(auto it = it2; it != it1; ++it) {
		val = *it;
	}
	ASSERT_TRUE(val == 3);
	ptr4.reset();
	val = 0;
	for(auto it = it2; it != it1; ++it) {
		val = *it;
	}
	ASSERT_TRUE(val == 3);
}

} /* namespace containers */
} /* namespace mce */
