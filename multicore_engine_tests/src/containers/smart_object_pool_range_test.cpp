/*
 * Multi-Core Engine project
 * File /multicore_engine_tests/src/containers/smart_object_pool_range_test.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <atomic>
#include <gtest.hpp>
#include <mce/containers/smart_object_pool_range.hpp>

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
	};
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

TEST(containers_smart_object_pool_range_test, construct) {
	typedef dummy_iterator<int> dit_t;
	int data[2 * 128];
	dit_t::pool_t dpool;
	dpool.block_count = 2;
	dit_t::block_t dblocks[2];

	dblocks[0].block_index = 0;
	dblocks[0].entries = data;
	dblocks[0].owning_pool = &dpool;
	dblocks[0].next_block = dblocks + 1;

	dblocks[1].block_index = 1;
	dblocks[1].entries = data + 128;
	dblocks[1].owning_pool = &dpool;
	dblocks[1].next_block = nullptr;

	dit_t it0(dit_t::target_type{dblocks[0].entries, dblocks}, &dpool);
	dit_t it1;

	smart_object_pool_range<dummy_iterator<int>> r1(it0, it1);

	ASSERT_TRUE(!r1.lower.limiter);
	ASSERT_TRUE(r1.lower.valid);
	ASSERT_TRUE(r1.lower.pool == &dpool);
	ASSERT_TRUE(r1.lower.target.containing_block == dblocks);
	ASSERT_TRUE(r1.lower.target.entry == data);

	ASSERT_TRUE(r1.upper.limiter);
	ASSERT_TRUE(!r1.upper.valid);
	ASSERT_TRUE(r1.upper.target.containing_block == nullptr);
	ASSERT_TRUE(r1.upper.target.entry == nullptr);
}

TEST(containers_smart_object_pool_range_test, split) {
	typedef dummy_iterator<int> dit_t;
	int data[2 * 128];
	dit_t::pool_t dpool;
	dpool.block_count = 2;
	dit_t::block_t dblocks[2];

	dblocks[0].block_index = 0;
	dblocks[0].entries = data;
	dblocks[0].owning_pool = &dpool;
	dblocks[0].next_block = dblocks + 1;

	dblocks[1].block_index = 1;
	dblocks[1].entries = data + 128;
	dblocks[1].owning_pool = &dpool;
	dblocks[1].next_block = nullptr;

	dit_t it0(dit_t::target_type{dblocks[0].entries, dblocks}, &dpool);
	dit_t it1;

	smart_object_pool_range<dummy_iterator<int>> r1(it0, it1);
	decltype(r1) r2(r1, tbb::split{});

	ASSERT_TRUE(!r1.lower.limiter);
	ASSERT_TRUE(r1.lower.valid);
	ASSERT_TRUE(r1.lower.pool == &dpool);
	ASSERT_TRUE(r1.lower.target.containing_block == dblocks);
	ASSERT_TRUE(r1.lower.target.entry == data);

	ASSERT_TRUE(r1.upper.limiter);
	ASSERT_TRUE(!r1.upper.valid);
	ASSERT_TRUE(r1.upper.pool == &dpool);
	ASSERT_TRUE(r1.upper.target.containing_block == dblocks + 1);
	ASSERT_TRUE(r1.upper.target.entry == data + 128);

	ASSERT_TRUE(!r2.lower.limiter);
	ASSERT_TRUE(r2.lower.valid);
	ASSERT_TRUE(r2.lower.pool == &dpool);
	ASSERT_TRUE(r2.lower.target.containing_block == dblocks + 1);
	ASSERT_TRUE(r2.lower.target.entry == data + 128);

	ASSERT_TRUE(r2.upper.limiter);
	ASSERT_TRUE(!r2.upper.valid);
	ASSERT_TRUE(r2.upper.target.containing_block == nullptr);
	ASSERT_TRUE(r2.upper.target.entry == nullptr);
}
TEST(containers_smart_object_pool_range_test, empty_positive) {
	typedef dummy_iterator<int> dit_t;
	int data[2 * 128];
	dit_t::pool_t dpool;
	dpool.block_count = 2;
	dit_t::block_t dblocks[2];

	dblocks[0].block_index = 0;
	dblocks[0].entries = data;
	dblocks[0].owning_pool = &dpool;
	dblocks[0].next_block = dblocks + 1;

	dblocks[1].block_index = 1;
	dblocks[1].entries = data + 128;
	dblocks[1].owning_pool = &dpool;
	dblocks[1].next_block = nullptr;

	dit_t it0(dit_t::target_type{dblocks[0].entries, dblocks}, &dpool);
	dit_t it1(dit_t::target_type{dblocks[0].entries, dblocks}, &dpool);

	smart_object_pool_range<dummy_iterator<int>> r1(it0, it1);

	ASSERT_TRUE(!r1.lower.limiter);
	ASSERT_TRUE(r1.lower.valid);
	ASSERT_TRUE(r1.lower.pool == &dpool);
	ASSERT_TRUE(r1.lower.target.containing_block == dblocks);
	ASSERT_TRUE(r1.lower.target.entry == data);

	ASSERT_TRUE(r1.upper.limiter);
	ASSERT_TRUE(!r1.upper.valid);
	ASSERT_TRUE(r1.upper.pool == &dpool);
	ASSERT_TRUE(r1.upper.target.containing_block == dblocks);
	ASSERT_TRUE(r1.upper.target.entry == data);

	ASSERT_TRUE(r1.empty());
}
TEST(containers_smart_object_pool_range_test, empty_negative) {
	typedef dummy_iterator<int> dit_t;
	int data[2 * 128];
	dit_t::pool_t dpool;
	dpool.block_count = 2;
	dit_t::block_t dblocks[2];

	dblocks[0].block_index = 0;
	dblocks[0].entries = data;
	dblocks[0].owning_pool = &dpool;
	dblocks[0].next_block = dblocks + 1;

	dblocks[1].block_index = 1;
	dblocks[1].entries = data + 128;
	dblocks[1].owning_pool = &dpool;
	dblocks[1].next_block = nullptr;

	dit_t it0(dit_t::target_type{dblocks[0].entries, dblocks}, &dpool);
	dit_t it1(dit_t::target_type{dblocks[0].entries + 1, dblocks}, &dpool);

	smart_object_pool_range<dummy_iterator<int>> r1(it0, it1);

	ASSERT_TRUE(!r1.lower.limiter);
	ASSERT_TRUE(r1.lower.valid);
	ASSERT_TRUE(r1.lower.pool == &dpool);
	ASSERT_TRUE(r1.lower.target.containing_block == dblocks);
	ASSERT_TRUE(r1.lower.target.entry == data);

	ASSERT_TRUE(r1.upper.limiter);
	ASSERT_TRUE(!r1.upper.valid);
	ASSERT_TRUE(r1.upper.pool == &dpool);
	ASSERT_TRUE(r1.upper.target.containing_block == dblocks);
	ASSERT_TRUE(r1.upper.target.entry == data + 1);

	ASSERT_TRUE(!r1.empty());
}

TEST(containers_smart_object_pool_range_test, divisibility_positive) {
	typedef dummy_iterator<int> dit_t;
	int data[2 * 128];
	dit_t::pool_t dpool;
	dpool.block_count = 2;
	dit_t::block_t dblocks[2];

	dblocks[0].block_index = 0;
	dblocks[0].entries = data;
	dblocks[0].owning_pool = &dpool;
	dblocks[0].next_block = dblocks + 1;

	dblocks[1].block_index = 1;
	dblocks[1].entries = data + 128;
	dblocks[1].owning_pool = &dpool;
	dblocks[1].next_block = nullptr;

	dit_t it0(dit_t::target_type{dblocks[0].entries, dblocks}, &dpool);
	dit_t it1(dit_t::target_type{dblocks[0].entries + 2, dblocks}, &dpool);

	smart_object_pool_range<dummy_iterator<int>> r1(it0, it1);

	ASSERT_TRUE(!r1.lower.limiter);
	ASSERT_TRUE(r1.lower.valid);
	ASSERT_TRUE(r1.lower.pool == &dpool);
	ASSERT_TRUE(r1.lower.target.containing_block == dblocks);
	ASSERT_TRUE(r1.lower.target.entry == data);

	ASSERT_TRUE(r1.upper.limiter);
	ASSERT_TRUE(!r1.upper.valid);
	ASSERT_TRUE(r1.upper.pool == &dpool);
	ASSERT_TRUE(r1.upper.target.containing_block == dblocks);
	ASSERT_TRUE(r1.upper.target.entry == data + 2);

	ASSERT_TRUE(r1.is_divisible());
}
TEST(containers_smart_object_pool_range_test, divisibility_negative) {
	typedef dummy_iterator<int> dit_t;
	int data[2 * 128];
	dit_t::pool_t dpool;
	dpool.block_count = 2;
	dit_t::block_t dblocks[2];

	dblocks[0].block_index = 0;
	dblocks[0].entries = data;
	dblocks[0].owning_pool = &dpool;
	dblocks[0].next_block = dblocks + 1;

	dblocks[1].block_index = 1;
	dblocks[1].entries = data + 128;
	dblocks[1].owning_pool = &dpool;
	dblocks[1].next_block = nullptr;

	dit_t it0(dit_t::target_type{dblocks[0].entries, dblocks}, &dpool);
	dit_t it1(dit_t::target_type{dblocks[0].entries + 1, dblocks}, &dpool);

	smart_object_pool_range<dummy_iterator<int>> r1(it0, it1);

	ASSERT_TRUE(!r1.lower.limiter);
	ASSERT_TRUE(r1.lower.valid);
	ASSERT_TRUE(r1.lower.pool == &dpool);
	ASSERT_TRUE(r1.lower.target.containing_block == dblocks);
	ASSERT_TRUE(r1.lower.target.entry == data);

	ASSERT_TRUE(r1.upper.limiter);
	ASSERT_TRUE(!r1.upper.valid);
	ASSERT_TRUE(r1.upper.pool == &dpool);
	ASSERT_TRUE(r1.upper.target.containing_block == dblocks);
	ASSERT_TRUE(r1.upper.target.entry == data + 1);

	ASSERT_TRUE(!r1.is_divisible());
}

} // namespace containers
} // namespace mce
