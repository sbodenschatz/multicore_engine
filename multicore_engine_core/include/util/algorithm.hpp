/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/algorithm.hpp
 * Copyright 2016-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_ALGORITHM_HPP_
#define UTIL_ALGORITHM_HPP_

/**
 * \file
 * Contains generic algorithms that are used in the code base but are not part of the algorithms set in the
 * STL.
 */

#include <cstddef>

namespace mce {
namespace util {

/// Merges adjacent elements using a merge operation if a predicate returns true and returns the new end.
/**
 * The function works on a range of iterators of type It.
 * The type It must fulfill the requirements of the ForwardIterator concept in the standard library.
 * The successors of elements that were merged into another are put into their new positions by
 * std::move_if_noexcept.
 * The elements therefore need to be either noexcept-moveable or copyable.
 *
 * In the following let T be the type of the range elements (decltype(*begin)).
 * The predicate function object needs to be callable with a signature compatible to bool(T,T) (usually it
 * should be bool(const T&,const T&)).
 * The merge function object needs to be callable with a signature of void(T&,T&).
 *
 * The algorithm works with O(number of elements) many internal iterations, predicate calls, merge calls, and
 * std::move_if_noexcept calls.
 */
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

/// Removes excess consecutive equal elements so that at most n equal elements per consecutive group are left.
/**
 * The function works on a range of iterators of type It.
 * The type It must fulfill the requirements of the ForwardIterator concept in the standard library.
 * The successors of removed elements are put into their new positions by std::move_if_noexcept.
 * The elements therefore need to be either noexcept-moveable or copyable.
 *
 * In the following let T be the type of the range elements (decltype(*begin)).
 * The eq function object checks elements for equality and must be callable with a signature compatible with
 * bool(T,T) (usually it should be bool(const T&,const T&)) and return true if the compared elements are
 * considered equal and false otherwise.
 *
 * The algorithm works with O(number of elements) many internal iterations, eq calls, and
 * std::move_if_noexcept calls.
 *
 * The algorithm returns an iterator to the new end of the range.
 */
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
			if(out != cur) {
				*out = std::move_if_noexcept(*cur);
			}
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
