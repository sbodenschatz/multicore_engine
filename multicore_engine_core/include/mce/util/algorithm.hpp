/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/algorithm.hpp
 * Copyright 2016-2018 by Stefan Bodenschatz
 */

#ifndef UTIL_ALGORITHM_HPP_
#define UTIL_ALGORITHM_HPP_

/**
 * \file
 * Contains generic algorithms that are used in the code base but are not part of the algorithms set in the
 * STL.
 */

#include <algorithm>
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

/// \brief Stable-sorts the elements in [val_begin,val_end) in the order their keys (obtained by the given key
/// function object) appear in [pref_begin,pref_end).
/**
 * The key function object must be callable with a const reference to the element type of [val_begin,val_end)
 * and return a value of a type that can be searched for in [pref_begin,pref_end) using std::find.
 *
 * Elements from [val_begin,val_end) with keys that don't appear in [pref_begin,pref_end) are sorted to the
 * end in stable order.
 * The function returns an iterator to the first such element or val_end if all element keys were found in
 * [pref_begin,pref_end).
 */
template <typename ItVal, typename ItPref, typename KeyFunc>
ItVal preference_sort(ItVal val_begin, ItVal val_end, ItPref pref_begin, ItPref pref_end, KeyFunc key) {
	auto pref = [&pref_begin, &pref_end](const auto& x) { return std::find(pref_begin, pref_end, x); };
	std::stable_sort(val_begin, val_end,
					 [&key, &pref](const auto& v0, const auto& v1) { return pref(key(v0)) < pref(key(v1)); });
	return std::find_if(val_begin, val_end,
						[&key, &pref, &pref_end](const auto& v) { return pref(key(v)) == pref_end; });
}

/// Stable-sorts the elements in [val_begin,val_end) in the order they appear in [pref_begin,pref_end).
/**
 * The element type of [val_begin,val_end) must be a type that can be searched for in [pref_begin,pref_end)
 * using std::find.
 *
 * Elements from [val_begin,val_end) that don't appear in [pref_begin,pref_end) are sorted to the end in
 * stable order.
 * The function returns an iterator to the first such element or val_end if all elements were found in
 * [pref_begin,pref_end).
 */
template <typename ItVal, typename ItPref>
ItVal preference_sort(ItVal val_begin, ItVal val_end, ItPref pref_begin, ItPref pref_end) {
	return preference_sort(val_begin, val_end, pref_begin, pref_end, [](auto v) { return v; });
}

/// \brief Stable-sorts the elements in range val_range in the order their keys (obtained by the given key
/// function object) appear in the range pref_range.
/**
 * The key function object must be callable with a const reference to the element type of val_range and return
 * a value of a type that can be searched for in pref_range using std::find.
 *
 * Elements from val_range with keys that don't appear in pref_range are sorted to the end in stable order.
 * The function returns an iterator to the first such element or val_range.end() if all element keys were
 * found in pref_range.
 */
template <typename ValRange, typename PrefRange, typename KeyFunc>
auto preference_sort(ValRange& val_range, const PrefRange& pref_range, KeyFunc key) {
	return preference_sort(val_range.begin(), val_range.end(), pref_range.begin(), pref_range.end(), key);
}

/// Stable-sorts the elements in range val_range in the order they appear in the range pref_range.
/**
 * The element type of val_range must be a type that can be searched for in pref_range using std::find.
 *
 * Elements from val_range that don't appear in pref_range are sorted to the end in stable order.
 * The function returns an iterator to the first such element or val_range.end() if all elements were found in
 * pref_range.
 */
template <typename ValRange, typename PrefRange>
auto preference_sort(ValRange& val_range, const PrefRange& pref_range) {
	return preference_sort(val_range.begin(), val_range.end(), pref_range.begin(), pref_range.end());
}

/// Describes a grouping for grouped_foreach.
template <typename EqPred, typename Pre, typename Post>
struct foreach_grouping {
	EqPred eq_pred; ///< A binary predicate on elements that tests if both belong to the same group.
	Pre pre;		///< A function object that is called on the group key at the start of a group.
	Post post;		///< A function object that is called on the group key at the end of a group.
};

/// Provides template argument deduction (in pre-C++17) for foreach_grouping.
template <typename EqPred, typename Pre, typename Post>
foreach_grouping<EqPred, Pre, Post> make_foreach_grouping(EqPred eq_pred, Pre pre, Post post) {
	return {eq_pred, pre, post};
}

namespace detail {

template <typename It, typename ElementFunc, typename ParentPred, typename Group, typename... Groups>
It grouped_foreach(It begin, It end, ElementFunc element_func, ParentPred parent_pred, Group group) {
	auto pred = [&parent_pred, &group](const auto& a, const auto& b) {
		return parent_pred(a, b) && group.eq_pred(a, b);
	};
	auto group_key = begin;
	group.pre(*group_key);
	do {
		element_func(*begin);
		++begin;
	} while((begin != end) && pred(*group_key, *begin));
	group.post(*group_key);
	return begin;
}

template <typename It, typename ElementFunc, typename ParentPred, typename Group, typename... Groups>
It grouped_foreach(It begin, It end, ElementFunc element_func, ParentPred parent_pred, Group group,
				   Groups... groups) {
	auto pred = [&parent_pred, &group](const auto& a, const auto& b) {
		return parent_pred(a, b) && group.eq_pred(a, b);
	};
	auto group_key = begin;
	group.pre(*group_key);
	do {
		begin = grouped_foreach(begin, end, element_func, pred, groups...);
	} while((begin != end) && pred(*group_key, *begin));
	group.post(*group_key);
	return begin;
}

} // namespace detail

/// \brief Applies element_func to each element in the sorted range [begin,end) and groups the elements into
/// nested groups defined by foreach_grouping objects in the groups parameter.
/**
 * The range is required to be sorted according to the grouping criteria.
 * The groupings are defined using foreach_grouping objects.
 * The helper function make_foreach_grouping provides template argument deduction for creating
 * foreach_grouping objects if no C++17 class template argument deduction is available.
 *
 * The foreach_grouping objects contain a binary predicate eq_pred that tests if two elements belong to the
 * same group.
 * When a group is started, the pre function object of the grouping is called.
 * When a group ends, the post function object of the grouping is called.
 *
 * At the start of a group, the iterator to the first element is stored as the iterator to the group key.
 * The group g ends when the current element is no longer equal to the group key according to the eq_pred of
 * any of the groupings not nested deeper than the grouping of g.
 * Only the groups for which eq_pred returns false and further nested groups are exited.
 *
 * The pre and post function objects are called using the group key as a parameter.
 * Because this is always the first element of the group, only the properties related to the grouping should
 * be evaluated (e.g. the member variables queried in eq_pred).
 */
template <typename It, typename ElementFunc, typename... Groups>
void grouped_foreach(It begin, It end, ElementFunc element_func, Groups&&... groups) {
	do {
		begin = detail::grouped_foreach(begin, end, element_func,
										[](const auto&, const auto&) { return true; }, groups...);
	} while(begin != end);
}

} // namespace util
} // namespace mce

#endif /* UTIL_ALGORITHM_HPP_ */
