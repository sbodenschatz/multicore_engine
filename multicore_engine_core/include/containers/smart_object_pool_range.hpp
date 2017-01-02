/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/smart_object_pool_range.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_SMART_OBJECT_POOL_RANGE_HPP_
#define CONTAINERS_SMART_OBJECT_POOL_RANGE_HPP_

#include <containers/smart_object_pool.hpp>
#include <tbb/tbb_stddef.h>

namespace mce {
namespace containers {

template <typename It>
struct smart_object_pool_range {
	It lower;
	It upper;
	smart_object_pool_range(It lower, It upper) noexcept : lower{lower}, upper{upper.make_limiter()} {
		if(!lower.target.containing_block) throw logic_exception("Start of block can't be end iterator.");
	}
	bool empty() const noexcept {
		return lower == upper;
	}
	bool is_divisible() const noexcept {
		auto x = lower;
		x++;
		if(x == upper) return false;
		x++;
		return x != upper;
	}
	smart_object_pool_range(smart_object_pool_range& other, tbb::split)
			: lower{other.lower}, upper{other.upper} {
		if(!lower.target.containing_block) throw logic_exception("Start of block can't be end iterator.");
		size_t i0 = lower.target.containing_block->block_index * block_size +
					(lower.target.entry - lower.target.containing_block->entries);
		size_t i1 = (lower.target.containing_block->owning_pool->block_count.load() + 1) * block_size;
		if(upper.target.containing_block) {
			i1 = upper.target.containing_block->block_index * block_size +
				 (upper.target.entry - upper.target.containing_block->entries);
		}
		size_t ih = i0 + (i1 - i0) / 2;
		size_t ib = ih / block_size;
		size_t ie = ih % block_size;
		auto cur_block = upper.target.containing_block;
		for(; cur_block && cur_block->block_index < ib; cur_block = cur_block->next_block) {
		}
		assert(cur_block);
		typename It::target_type tar{cur_block->entries + ie, cur_block};
		It it(tar, lower.target.containing_block->owning_pool);
		lower = it;
		other.upper = it.make_limiter();
	}
};

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_SMART_OBJECT_POOL_RANGE_HPP_ */
