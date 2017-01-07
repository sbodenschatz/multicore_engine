/*
 * Multi-Core Engine project
 * File /multicore_engine_core_tests/src/containers/smart_object_pool_range_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <atomic>
#include <boost/test/unit_test.hpp>
#include <containers/smart_object_pool_range.hpp>

namespace mce {
namespace containers {

template <typename T>
struct dummy_iterator {
	struct pool {
		std::atomic<size_t> block_count;
	};
	struct block {
		T* entries;
		size_t block_index;
		pool* owning_pool;
		block* next_block;
	} b;
	struct target_type {
		T* entry;
		block* containing_block;
	};
	target_type target;
	// typename dummy_iterator<T>::target_t target_type;
	bool limiter = false;
	dummy_iterator make_limiter() const {
		dummy_iterator i;
		i.limiter = true;
		return i;
	}
	static size_t pool_block_size_;
	static size_t pool_block_size() {
		return pool_block_size_;
	}
	dummy_iterator() {
		// TODO Implement
	}
	dummy_iterator(const target_type&, pool*) {
		// TODO Implement
	}
	static void skip_until_valid(target_type&) {
		// TODO Implement
	}
	bool operator>=(const dummy_iterator<T>&) const {
		// TODO Implement
		return false;
	}
	bool operator==(const dummy_iterator<T>&) const {
		// TODO Implement
		return false;
	}
	bool operator!=(const dummy_iterator<T>&) const {
		// TODO Implement
		return false;
	}
	void operator++() const {
		// TODO Implement
	}
	void operator++(int)const {
		// TODO Implement
	}
};

template <typename T>
size_t dummy_iterator<T>::pool_block_size_ = 128;

BOOST_AUTO_TEST_SUITE(containers)
BOOST_AUTO_TEST_SUITE(smart_object_pool_range_test)

BOOST_AUTO_TEST_CASE(construct) {
	smart_object_pool_range<dummy_iterator<int>> r1(dummy_iterator<int>{}, dummy_iterator<int>{});
}
BOOST_AUTO_TEST_CASE(split) {
	smart_object_pool_range<dummy_iterator<int>> r1(dummy_iterator<int>{}, dummy_iterator<int>{});
	decltype(r1) r2(r1, tbb::split{});
}
BOOST_AUTO_TEST_CASE(empty) {
	smart_object_pool_range<dummy_iterator<int>> r1(dummy_iterator<int>{}, dummy_iterator<int>{});
	r1.empty();
}
BOOST_AUTO_TEST_CASE(divisibility) {
	smart_object_pool_range<dummy_iterator<int>> r1(dummy_iterator<int>{}, dummy_iterator<int>{});
	r1.is_divisible();
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

} // namespace containers
} // namespace mce
