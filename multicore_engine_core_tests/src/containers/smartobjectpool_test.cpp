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

namespace mce {
namespace containers {

struct smart_object_pool_fixture {
	typedef long long element;
	mce::containers::smart_object_pool<element> sop;
	smart_object_pool_fixture() {
	}
	~smart_object_pool_fixture() {
	}
};

BOOST_AUTO_TEST_SUITE(containers)
BOOST_FIXTURE_TEST_SUITE(smart_object_pool_test, smart_object_pool_fixture)

BOOST_AUTO_TEST_CASE(emplace_and_destroy_one) {
	auto ptr = sop.emplace(42);
	BOOST_CHECK(*ptr == 42);
	BOOST_CHECK(sop.size()==1);
	ptr.reset();
	BOOST_CHECK(sop.size()==0);
	BOOST_CHECK(sop.empty());
}

BOOST_AUTO_TEST_CASE(emplace_and_destroy_many) {
	std::vector<smart_pool_ptr<element>> elem_ptrs;
	for(int i = 0; i<512; ++i){
		auto ptr=sop.emplace(i);
		elem_ptrs.emplace_back(ptr);
		BOOST_CHECK(*ptr == i);
	}
	BOOST_CHECK(sop.size()==512);
	for(int i =0;i<512;++i){
		BOOST_CHECK(*(elem_ptrs[i])==i);
	}
	elem_ptrs.clear();
	BOOST_CHECK(sop.size()==0);
}

BOOST_AUTO_TEST_CASE(iterator_holds_object) {
	auto ptr = sop.emplace(42);
	BOOST_CHECK(*ptr == 42);
	BOOST_CHECK(sop.size()==1);
	auto it=sop.begin();
	ptr.reset();
	BOOST_CHECK(sop.size()==1);
	it=decltype(sop)::iterator();
	BOOST_CHECK(sop.size()==0);
	BOOST_CHECK(sop.empty());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} /* namespace containers */
} /* namespace mce */
