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

namespace test {

template <typename T>
struct dummy_iterator {
	struct pool_t {
		std::atomic<size_t> block_count;
	};
	struct block_t {
		T* entries;
		size_t block_index;
		pool_t* owning_pool;
		block_t* next_block;
	} b;
	struct target_type {
		T* entry;
		block_t* containing_block;
	};
	target_type target;
	pool_t* pool;
	bool limiter = false;
	bool valid = false;
	dummy_iterator make_limiter() const {
		dummy_iterator i = *this;
		i.limiter = true;
		i.valid = false;
		return i;
	}
	static size_t pool_block_size_;
	static size_t pool_block_size() {
		return pool_block_size_;
	}
	dummy_iterator() : target{nullptr, nullptr}, pool{nullptr} {}
	dummy_iterator(const target_type& t, pool_t* p) : target{t}, pool{p} {
		skip_until_valid();
	}
	struct no_skip_tag {};
	dummy_iterator(const target_type& t, pool_t* p, no_skip_tag) : target{t}, pool{p} {}
	void skip_until_valid() {
		valid = true;
		if(target.entry && target.containing_block && pool) {
			if(target.entry >= target.containing_block->entries + pool_block_size()) {
				target.containing_block = target.containing_block->next_block;
				if(target.containing_block) {
					target.entry = target.containing_block->entries;
				} else {
					valid = false;
				}
			}
		} else {
			valid = false;
		}
	}
	bool operator<(const dummy_iterator<T>& other) const {
		if(limiter) return false;
		if(!target.containing_block) return false;
		if(!other.target.containing_block) return true;
		return std::tie(target.containing_block->block_index, target.entry) <
			   std::tie(other.target.containing_block->block_index, other.target.entry);
	}
	bool operator>(const dummy_iterator<T>& other) const {
		return other < *this;
	}

	bool operator<=(const dummy_iterator<T>& other) const {
		return (*this < other) || (*this == other);
	}
	bool operator>=(const dummy_iterator<T>& other) const {
		return (*this > other) || (*this == other);
	}
	bool operator==(const dummy_iterator<T>& it) const {
		return (it.target.entry == target.entry && it.target.containing_block == target.containing_block) ||
			   (limiter && target.containing_block && it.target.containing_block &&
				target.containing_block->block_index <= it.target.containing_block->block_index &&
				target.entry <= it.target.entry) ||
			   (it.limiter && target.containing_block && it.target.containing_block &&
				it.target.containing_block->block_index <= target.containing_block->block_index &&
				it.target.entry <= target.entry);
	}
	bool operator!=(const dummy_iterator<T>& it) const {
		return !(*this == it);
	}
	dummy_iterator<T> operator++() {
		target.entry++;
		skip_until_valid();
		return *this;
	}
	dummy_iterator<T> operator++(int) {
		auto it = *this;
		this->operator++();
		return it;
	}
};

template <typename T>
size_t dummy_iterator<T>::pool_block_size_ = 128;

} // namespace test

using namespace test;

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
