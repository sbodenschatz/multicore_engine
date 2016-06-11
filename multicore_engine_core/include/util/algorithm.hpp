/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/algorithm.hpp
 * Copyright 2016 by Stefan Bodenschatz
 */

#ifndef UTIL_ALGORITHM_HPP_
#define UTIL_ALGORITHM_HPP_

namespace mce {
namespace util {

template <typename It, typename Pred, typename Merge>
It merge_adjacent_if(It begin, It end, Pred predicate, Merge merge) {
	if(begin == end) return end;
	It cur = begin;
	It next = cur;
	++next;
	while(next != end) {
		if(predicate(*cur, *next)) {
			merge(*cur, *next);
		} else {
			++cur;
			if(cur != next) {
				*cur = std::move_if_noexcept(*next);
			}
		}
		++next;
	}
	++cur;
	return cur;
}

template <typename It, typename Eq>
It n_unique(It begin, It end, Eq eq, size_t n) {
	if(begin == end) return end;
	It cur = begin;
	It group_start = cur;
	It out = begin;
	size_t i = 0;
	while(cur != end) {
		if(!eq(*group_start, *cur)) {
			i = 0;
			group_start = cur;
		}
		if(i < n) {
			*out = std::move_if_noexcept(*cur);
			++out;
		}
		++i;
		++cur;
	}
	return out;
}

} // namespace util
} // namespace mce

#endif /* UTIL_ALGORITHM_HPP_ */
