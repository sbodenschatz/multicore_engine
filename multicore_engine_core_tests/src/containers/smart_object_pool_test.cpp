/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/containers/smartobjectpool_test.cpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#include <containers/smart_object_pool.hpp>
#include <boost/test/unit_test.hpp>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <future>

namespace mce {
namespace containers {

struct smart_object_pool_fixture {
	struct X {
		long long x;
		X(const long long& o) : x(o) {}
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
	smart_object_pool_fixture() {}
	~smart_object_pool_fixture() {}
};

BOOST_AUTO_TEST_SUITE(containers)
BOOST_FIXTURE_TEST_SUITE(smart_object_pool_test, smart_object_pool_fixture)

BOOST_AUTO_TEST_CASE(emplace_and_destroy_one) {
	auto ptr = sop.emplace(42);
	BOOST_CHECK(*ptr == 42);
	BOOST_CHECK(sop.size() == 1);
	ptr.reset();
	BOOST_CHECK(sop.size() == 0);
	BOOST_CHECK(sop.empty());
}

BOOST_AUTO_TEST_CASE(emplace_and_destroy_many) {
	std::vector<smart_pool_ptr<element>> elem_ptrs;
	for(int i = 0; i < 512; ++i) {
		auto ptr = sop.emplace(i);
		elem_ptrs.emplace_back(ptr);
		BOOST_CHECK(*ptr == i);
	}
	BOOST_CHECK(sop.size() == 512);
	for(int i = 0; i < 512; ++i) {
		BOOST_CHECK(*(elem_ptrs[i]) == i);
	}
	elem_ptrs.clear();
	BOOST_CHECK(sop.size() == 0);
}

BOOST_AUTO_TEST_CASE(iterator_holds_object) {
	auto ptr = sop.emplace(42);
	BOOST_CHECK(*ptr == 42);
	BOOST_CHECK(sop.size() == 1);
	auto it = sop.begin();
	ptr.reset();
	BOOST_CHECK(sop.size() == 1);
	it = decltype(sop)::iterator();
	BOOST_CHECK(sop.size() == 0);
	BOOST_CHECK(sop.empty());
}

BOOST_AUTO_TEST_CASE(rescue_object) {
	auto ptr = sop.emplace(42);
	BOOST_CHECK(*ptr == 42);
	BOOST_CHECK(sop.size() == 1);
	auto it = sop.begin();
	weak_pool_ptr<element> wptr = ptr;
	ptr.reset();
	ptr = wptr.lock();
	BOOST_CHECK(ptr);
	BOOST_CHECK(sop.size() == 1);
	it = decltype(sop)::iterator();
	BOOST_CHECK(*ptr == 42);
	ptr.reset();
	BOOST_CHECK(sop.size() == 0);
	BOOST_CHECK(sop.empty());
}

BOOST_AUTO_TEST_CASE(mt_emplace_and_destroy_many) {
	auto t1 = std::chrono::high_resolution_clock::now();
	sop.reserve(128 * 0x1000);
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff1 = t2 - t1;
	std::cout << diff1.count() << std::endl;
	std::vector<std::future<bool>> futures;
	for(int t = 0; t < 256; ++t) {
		futures.emplace_back(std::async(std::launch::async, [&](int t) {
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
		}, t));
	}
	for(auto& f : futures) {
		BOOST_CHECK(f.get());
	}
	auto t3 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff2 = t3 - t2;
	std::cout << diff2.count() << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} /* namespace containers */
} /* namespace mce */
