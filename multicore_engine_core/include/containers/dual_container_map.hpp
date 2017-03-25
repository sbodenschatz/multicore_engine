/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/dual_container_map.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_DUAL_CONTAINER_MAP_HPP_
#define CONTAINERS_DUAL_CONTAINER_MAP_HPP_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

namespace mce {
namespace containers {
/// Provides common functionality for dual_container_map and dual_container_multimap.
template <typename Map, template <typename> class Container, typename Key, typename Value,
		  typename Compare = std::less<>>
class dual_container_map_base {
protected:
	Container<Key> keys;
	Container<Value> values;
	Compare compare;

	template <typename... Args>
	explicit dual_container_map_base(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Container<Key>, Args...>::value&& std::is_nothrow_constructible<
					Container<Value>, Args...>::value&& std::is_nothrow_copy_constructible<Compare>::value)
			: keys(std::forward<Args>(args)...), values(std::forward<Args>(args)...), compare(compare) {}
	template <typename... Args>
	explicit dual_container_map_base(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Container<Key>, Args...>::value&&
					std::is_nothrow_constructible<Container<Value>, Args...>::value &&
			(std::is_nothrow_copy_constructible<Compare>::value ||
			 std::is_nothrow_move_constructible<Compare>::value))
			: keys(std::forward<Args>(args)...), values(std::forward<Args>(args)...),
			  compare(std::move_if_noexcept(compare)) {}
	dual_container_map_base(const dual_container_map_base& other) noexcept(
			std::is_nothrow_copy_constructible<Container<Key>>::value&& std::is_nothrow_copy_constructible<
					Container<Value>>::value&& std::is_nothrow_copy_constructible<Compare>::value)
			: keys(other.keys), values(other.values), compare(other.compare) {}
	dual_container_map_base(dual_container_map_base&& other) noexcept(
			(std::is_nothrow_copy_constructible<Container<Key>>::value ||
			 std::is_nothrow_move_constructible<Container<Key>>::value) &&
			(std::is_nothrow_copy_constructible<Container<Value>>::value ||
			 std::is_nothrow_move_constructible<Container<Value>>::value) &&
			(std::is_nothrow_copy_constructible<Compare>::value ||
			 std::is_nothrow_move_constructible<Compare>::value))
			: keys(std::move_if_noexcept(other.keys)), values(std::move_if_noexcept(other.values)),
			  compare(std::move_if_noexcept(other.compare)) {}
	dual_container_map_base& operator=(const dual_container_map_base& other) noexcept(
			std::is_nothrow_copy_assignable<Container<Key>>::value&& std::is_nothrow_copy_assignable<
					Container<Value>>::value&& std::is_nothrow_copy_assignable<Compare>::value) {
		try {
			compare = other.compare;
			keys.reserve(other.keys.size());
			values.reserve(other.values.size());
			keys = other.keys;
			values = other.values;
		} catch(...) {
			keys.clear();
			values.clear();
			throw;
		}
		return *this;
	}
	dual_container_map_base& operator=(dual_container_map_base&& other) noexcept(
			std::is_nothrow_copy_assignable<Container<Key>>::value&& std::is_nothrow_copy_assignable<
					Container<Value>>::value&& std::is_nothrow_copy_assignable<Compare>::value) {
		assert(this == &other);
		try {
			compare = std::move_if_noexcept(other.compare);
			keys = std::move_if_noexcept(other.keys);
			values = std::move_if_noexcept(other.values);
		} catch(...) {
			keys.clear();
			values.clear();
			throw;
		}
		return *this;
	}

public:
	/// Proxy type for wrapping temporaries in operator-> on iterators.
	template <typename T>
	struct member_access_wrapper {
		T value;
		template <typename... Args>
		explicit member_access_wrapper(Args&&... args) : value{std::forward<Args>(args)...} {}
		T* operator->() noexcept {
			return &value;
		}
	};

	/// Iterator type for dual_container_map and dual_container_multimap.
	/**
	 * Provides access to the key-value-pairs in the containers.
	 * Satisfies the requirements of the RandomAccessIterator concept.
	 * Note that access is provided by a pair of references instead of by a reference to a pair.
	 * The latter would not be implementable because no such pair physically exists as the elements exist in
	 * different containers and the connection is only formed by matching indexes.
	 */
	template <typename It_Map, typename It_Key, typename It_Value>
	class iterator_ : public std::iterator<std::random_access_iterator_tag, std::pair<It_Key&, It_Value&>> {
		size_t index;
		It_Map* map;

		iterator_(size_t index, It_Map* map) noexcept : index(index), map(map) {}

	public:
		template <typename M, typename IK, typename IV>
		friend class iterator_;
		friend It_Map;
		template <typename M, template <typename> class Cont, typename K, typename V, typename Comp>
		friend class dual_container_map_base;
		typedef typename iterator_::value_type reference;
		iterator_() = delete;
		iterator_(const iterator_<std::remove_const_t<It_Map>, It_Key, std::remove_const_t<It_Value>>&
						  it) noexcept : index(it.index),
										 map(it.map) {}
		iterator_& operator=(const iterator_<std::remove_const_t<It_Map>, It_Key,
											 std::remove_const_t<It_Value>>& it) noexcept {
			index = it.index;
			map = it.map;
			return *this;
		}

		/// Returns the currently referenced key and value as a pair of references.
		std::pair<It_Key&, It_Value&> operator*() const noexcept {
			return std::make_pair(std::ref(map->keys[index]), std::ref(map->values[index]));
		}

		/// Allows member access to the currently referenced key and value as a pair of references.
		member_access_wrapper<std::pair<It_Key&, It_Value&>> operator->() const noexcept {
			return member_access_wrapper<std::pair<It_Key&, It_Value&>>(operator*());
		}

		/// Returns true if and only if the *this and it refer to the same key-value-element of the same map.
		bool operator==(const iterator_& it) const noexcept {
			return it.index == index && it.map == map;
		}

		/// Returns false if and only if the *this and it refer to the same key-value-element of the same map.
		bool operator!=(const iterator_& it) const noexcept {
			return !(*this == it);
		}

		/// Advances the iterator to the next element and returns the new iterator.
		iterator_& operator++() noexcept {
			++index;
			return *this;
		}

		/// Advances the iterator to the next element and returns the old iterator.
		iterator_ operator++(int)noexcept {
			auto it = *this;
			this->operator++();
			return it;
		}

		/// Advances the iterator to the previous element and returns the new iterator.
		iterator_& operator--() noexcept {
			--index;
			return *this;
		}
		/// Advances the iterator to the previous element and returns the old iterator.
		iterator_ operator--(int)noexcept {
			auto it = *this;
			this->operator--();
			return it;
		}

		/// Advances the iterator n elements ahead.
		iterator_& operator+=(typename iterator_::difference_type n) noexcept {
			index += n;
			return *this;
		}

		/// Advances the iterator n elements back.
		iterator_& operator-=(typename iterator_::difference_type n) noexcept {
			index -= n;
			return *this;
		}

		/// Returns a copy of it advanced n elements ahead.
		friend iterator_ operator+(iterator_ it, typename iterator_::difference_type n) noexcept {
			return it += n;
		}

		/// Returns a copy of it advanced n elements ahead.
		friend iterator_ operator+(typename iterator_::difference_type n, iterator_ it) noexcept {
			return it += n;
		}

		/// Returns a copy of it advanced n elements before.
		friend iterator_ operator-(iterator_ it, typename iterator_::difference_type n) noexcept {
			return it -= n;
		}

		/// Returns a copy of it advanced n elements before.
		friend typename iterator_::difference_type operator-(iterator_ it1, iterator_ it2) noexcept {
			return it1.index - it2.index;
		}

		/// Allows access to the key and value pair n elements ahead as a pair of references.
		std::pair<It_Key&, It_Value&> operator[](int n) noexcept {
			return *((*this) + n);
		}

		/// \brief Returns true if and only if the referenced element of *this is in the same map as the
		/// referenced element of other and is located before it.
		/**
		 * Note that iterators from different maps compare false for <,>,>=, and <=.
		 */
		bool operator<(iterator_ other) noexcept {
			return (map == other.map) && (index < other.index);
		}

		/// \brief Returns true if and only if the referenced element of *this is in the same map as the
		/// referenced element of other and is located after it.
		/**
		 * Note that iterators from different maps compare false for <,>,>=, and <=.
		 */
		bool operator>(iterator_ other) noexcept {
			return (map == other.map) && (index > other.index);
		}

		/// \brief Returns true if and only if the referenced element of *this is in the same map as the
		/// referenced element of other and is located before it or if both reference the same element.
		/**
		 * Note that iterators from different maps compare false for <,>,>=, and <=.
		 */
		bool operator<=(iterator_ other) noexcept {
			return (map == other.map) && (index <= other.index);
		}

		/// \brief Returns true if and only if the referenced element of *this is in the same map as the
		/// referenced element of other and is located after it or if both reference the same element.
		/**
		 * Note that iterators from different maps compare false for <,>,>=, and <=.
		 */
		bool operator>=(iterator_ other) noexcept {
			return (map == other.map) && (index >= other.index);
		}
	};

	/// RandomAccessIterator
	typedef iterator_<Map, const Key, Value> iterator;
	/// Constant RandomAccessIterator
	typedef iterator_<const Map, const Key, const Value> const_iterator;

	/// Reverse RandomAccessIterator
	typedef std::reverse_iterator<iterator> reverse_iterator;
	/// Reverse constant RandomAccessIterator
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	/// Removes all key-value-elements from the map and thus makes it empty.
	void clear() noexcept {
		keys.clear();
		values.clear();
	}

	/// Removes the key-value-element referenced by pos from the map.
	/**
	 * Because the elements after the removed element need to be moved to close the gap and those moves might
	 * throw exceptions this operation can only provide basic exception guarantee. If an exception occurs
	 * during removal the map will be left in an empty but valid state.
	 */
	iterator erase(iterator pos) {
		return erase(const_iterator(pos));
	}

	/// Removes the key-value-element referenced by pos from the map.
	/**
	 * Because the elements after the removed element need to be moved to close the gap and those moves might
	 * throw exceptions this operation can only provide the basic exception guarantee. If an exception occurs
	 * during removal the map will be left in an empty but valid state.
	 */
	iterator erase(const_iterator pos) {
		try {
			auto it = keys.erase(keys.begin() + pos.index);
			values.erase(values.begin() + pos.index);
			return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
		} catch(...) {
			keys.clear();
			values.clear();
			throw;
		}
	}

	/// Swaps the contents of *this with other by swapping their components.
	/**
	 * Because exceptions thrown during one of the component swaps may leave the map in an invalid or
	 * inconsistent state the operation can only provide the basic exception guarantee. If an exception is
	 * thrown by one of the component swaps both maps will be put into an empty but valid state.
	 */
	void swap(dual_container_map_base& other) {
		try {
			using std::swap;
			swap(compare, other.compare);
			swap(keys, other.keys);
			swap(values, other.values);
		} catch(...) {
			keys.clear();
			values.clear();
			other.keys.clear();
			other.values.clear();
			throw;
		}
	}

	/// \brief Looks up the element(s) with the given key and returns an iterator to the first such element or
	/// an past-end-iterator if no such element exists.
	iterator find(const Key& key) {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		if(it != keys.end())
			if(compare(key, *it)) it = keys.end();
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}

	/// \brief Looks up the element(s) with the given key and returns a const_iterator to the first such
	/// element or an past-end-iterator if no such element exists.
	const_iterator find(const Key& key) const {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		if(it != keys.end())
			if(compare(key, *it)) it = keys.end();
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}

	/// \brief Looks up the element(s) with a key that compares equal to the given key and returns an iterator
	/// to the first such element or an past-end-iterator if no such element exists.
	/**
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator find(const K& key) {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		if(it != keys.end())
			if(compare(key, *it)) it = keys.end();
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}

	/// \brief Looks up the element(s) with a key that compares equal to the given key and returns a
	/// const_iterator to the first such element or an past-end-iterator if no such element exists.
	/**
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator find(const K& key) const {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		if(it != keys.end())
			if(compare(key, *it)) it = keys.end();
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}

	/// \brief Returns an iterator referencing the first element in the map whose key is not less than the
	/// given key or a past-end-iterator if no such element exist.
	iterator lower_bound(const Key& key) {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}

	/// \brief Returns a const_iterator referencing the first element in the map whose key is not less than
	/// the given key or a past-end-iterator if no such element exist.
	const_iterator lower_bound(const Key& key) const {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}

	/// \brief Returns an iterator referencing the first element in the map whose key is not less than the
	/// given key or a past-end-iterator if no such element exist.
	/**
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator lower_bound(const K& key) {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}

	/// \brief Returns a const_iterator referencing the first element in the map whose key is not less than
	/// the given key or a past-end-iterator if no such element exist.
	/**
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator lower_bound(const K& key) const {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}

	/// \brief Returns an iterator referencing the first element in the map whose key is greater than the
	/// given key or a past-end-iterator if no such element exist.
	iterator upper_bound(const Key& key) {
		auto it = std::upper_bound(keys.begin(), keys.end(), key, compare);
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}

	/// \brief Returns a const_iterator referencing the first element in the map whose key is greater than the
	/// given key or a past-end-iterator if no such element exist.
	const_iterator upper_bound(const Key& key) const {
		auto it = std::upper_bound(keys.begin(), keys.end(), key, compare);
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}

	/// \brief Returns an iterator referencing the first element in the map whose key is greater than the
	/// given key or a past-end-iterator if no such element exist.
	/**
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator upper_bound(const K& key) {
		auto it = std::upper_bound(keys.begin(), keys.end(), key, compare);
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}

	/// \brief Returns a const_iterator referencing the first element in the map whose key is greater than the
	/// given key or a past-end-iterator if no such element exist.
	/**
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator upper_bound(const K& key) const {
		auto it = std::upper_bound(keys.begin(), keys.end(), key, compare);
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}

	/// Returns the range of elements with a key matching the given key.
	/**
	 * Equivalent to std::make_pair(lower_bound(key), upper_bound(key))
	 */
	std::pair<iterator, iterator> equal_range(const Key& key) {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}

	/// Returns the range of elements with a key matching the given key.
	/**
	 * Equivalent to std::make_pair(lower_bound(key), upper_bound(key))
	 */
	std::pair<const_iterator, const_iterator> equal_range(const Key& key) const {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}

	/// Returns the range of elements with a key matching the given key.
	/**
	 * Equivalent to std::make_pair(lower_bound(key), upper_bound(key))
	 *
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K>
	std::pair<iterator, iterator> equal_range(const K& key) {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}

	/// Returns the range of elements with a key matching the given key.
	/**
	 * Equivalent to std::make_pair(lower_bound(key), upper_bound(key))
	 *
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K>
	std::pair<const_iterator, const_iterator> equal_range(const K& key) const {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}

	/// Returns whether the map is empty (true) or not (false).
	bool empty() const noexcept {
		return keys.empty();
	}

	/// Returns the number of elements (key-value-pairs) in the map.
	size_t size() const noexcept {
		return keys.size();
	}

	/// Returns an iterator to the first key-value-pair in the map or a past-end-iterator if the map is empty.
	iterator begin() noexcept {
		return iterator(0, static_cast<Map*>(this));
	}

	/// \brief Returns a const_iterator to the first key-value-pair in the map or a past-end-iterator if the
	/// map is empty.
	const_iterator begin() const noexcept {
		return const_iterator(0, static_cast<const Map*>(this));
	}

	/// \brief Returns a const_iterator to the first key-value-pair in the map or a past-end-iterator if the
	/// map is empty.
	const_iterator cbegin() const noexcept {
		return const_iterator(0, static_cast<const Map*>(this));
	}

	/// Returns a past-end-iterator for the map.
	iterator end() noexcept {
		return iterator(keys.size(), static_cast<Map*>(this));
	}

	/// Returns a constant past-end-iterator for the map.
	const_iterator end() const noexcept {
		return const_iterator(keys.size(), static_cast<const Map*>(this));
	}

	/// Returns a constant past-end-iterator for the map.
	const_iterator cend() const noexcept {
		return const_iterator(keys.size(), static_cast<const Map*>(this));
	}

	/// \brief Return a reverse iterator to the last key-value-pair in the map or a
	/// past-start-reverse-iterator if the map is empty.
	reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	/// \brief Return a constant reverse iterator to the last key-value-pair in the map or a
	/// past-start-reverse-iterator if the map is empty.
	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	/// \brief Return a constant reverse iterator to the last key-value-pair in the map or a
	/// past-start-reverse-iterator if the map is empty.
	const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}

	/// Returns a past-start-reverse-iterator for the map.
	reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	/// Returns a constant past-start-reverse-iterator for the map.
	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

	/// Returns a constant past-start-reverse-iterator for the map.
	const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}
};

/// \brief Provides a non-member ADL swap by calling the member dual_container_map_base::swap.
template <typename Map, template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(dual_container_map_base<Map, Container, Key, Value, Compare>& m1,
		  dual_container_map_base<Map, Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

/// \brief Implements a map by keeping the keys and the values in separate Container objects, key-value-pairs
/// are linked by matching indexes.
/**
 * The interface of this class resembles that of the STL map containers.
 * But iterators return (by value) a pair of references to the key and value instead of a reference to a pair
 * of the values of the keys and the values. This means taking the dereferenced value into auto by value still
 * yields a pair of references which can modify the value in the map.
 */
template <template <typename> class Container, typename Key, typename Value, typename Compare = std::less<>>
class dual_container_map : public dual_container_map_base<dual_container_map<Container, Key, Value, Compare>,
														  Container, Key, Value, Compare> {

	typedef dual_container_map_base<dual_container_map<Container, Key, Value, Compare>, Container, Key, Value,
									Compare>
			Base;

public:
	/// \brief Constructs a dual_container_map by forwarding the given parameters to both constructors of
	/// the underlying container types and default-constructing the Compare object.
	template <typename... Args>
	explicit dual_container_map(Args&&... args) noexcept(
			std::is_nothrow_default_constructible<Compare>::value&&
					std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: dual_container_map(Compare(), std::forward<Args>(args)...) {}
	/// \brief Constructs a dual_container_map by forwarding the given parameters to both constructors of
	/// the underlying container types and copying the Compare object.
	template <typename... Args>
	explicit dual_container_map(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(compare, std::forward<Args>(args)...) {}
	/// \brief Constructs a dual_container_map by forwarding the given parameters to both constructors of
	/// the underlying container types and moving the Compare object.
	template <typename... Args>
	explicit dual_container_map(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(std::move_if_noexcept(compare), std::forward<Args>(args)...) {}
	/// Copy-constructs a dual_container_map from other.
	dual_container_map(const dual_container_map& other) noexcept(
			std::is_nothrow_copy_constructible<Base>::value)
			: Base(other) {}
	/// Move-assigns a dual_container_map from other.
	dual_container_map(dual_container_map&& other) noexcept(std::is_nothrow_move_constructible<Base>::value)
			: Base(std::move(other)) {}
	/// Copy-assigns a dual_container_map from other.
	dual_container_map&
	operator=(const dual_container_map& other) noexcept(std::is_nothrow_copy_assignable<Base>::value) {
		Base::operator=(other);
		return *this;
	}
	/// Move-assigns a dual_container_map from other.
	dual_container_map&
	operator=(dual_container_map&& other) noexcept(std::is_nothrow_move_assignable<Base>::value) {
		Base::operator=(std::move(other));
		return *this;
	}

	/// RandomAccessIterator
	typedef typename Base::iterator iterator;
	/// Constant RandomAccessIterator
	typedef typename Base::const_iterator const_iterator;
	/// Reverse RandomAccessIterator
	typedef typename Base::reverse_iterator reverse_iterator;
	/// Reverse constant RandomAccessIterator
	typedef typename Base::const_reverse_iterator const_reverse_iterator;

	/// \brief Inserts a new element with the given key and value by forward-constructing them only if there
	/// is no element with the given key already in the map.
	/**
	 * The function returns an iterator to the inserted element and true if the element was inserted or an
	 * iterator to the existing element and false otherwise.
	 *
	 * The function reserves the required memory before modifying the underlying containers to leave the map
	 * unmodified if an exception is thrown while looking up the position for the new element or while
	 * allocating memory. However since the insertion requires moving the elements after the insertion
	 * position and those moves might throw, only the basic exception guarantee can be provided. If an
	 * exception is thrown while inserting the key or the value the map is left in an empty but valid state.
	 */
	template <typename K, typename V>
	std::pair<iterator, bool> insert(K&& key, V&& value) {
		auto it_after_key = std::upper_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		auto it_key = it_after_key;
		if(it_key != this->keys.begin()) --it_key;
		auto key_index = std::distance(this->keys.begin(), it_key);
		if(!this->compare(*it_key, key) && !this->compare(key, *it_key))
			return std::make_pair(iterator(key_index, this), false);
		auto index = std::distance(this->keys.begin(), it_after_key);
		auto it_after_value = this->values.begin() + index;
		this->keys.reserve(this->keys.size() + 1);
		this->values.reserve(this->values.size() + 1);
		try {
			this->keys.emplace(it_after_key, std::forward<K>(key));
			this->values.emplace(it_after_value, std::forward<V>(value));
			return std::make_pair(iterator(index, this), true);
		} catch(...) {
			this->keys.clear();
			this->values.clear();
			throw;
		}
	}

	/// Assigns the given value to the given key or inserts it for the given key if the key doesn't exist.
	/**
	 * The function returns an iterator to the inserted element and true if the element was new or an
	 * iterator to the assigned element and false otherwise.
	 *
	 * The function reserves the required memory before modifying the underlying containers to leave the map
	 * unmodified if an exception is thrown while looking up the position for the new element or while
	 * allocating memory. However since the insertion requires moving the elements after the insertion
	 * position and those moves might throw, only the basic exception guarantee can be provided. If an
	 * exception is thrown while inserting the key or the value the map is left in an empty but valid state.
	 */
	template <typename K, typename V>
	std::pair<iterator, bool> insert_or_assign(K&& key, V&& value) {
		auto it_after_key = std::upper_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		auto it_key = it_after_key;
		if(it_key != this->keys.begin()) --it_key;
		auto key_index = std::distance(this->keys.begin(), it_key);
		if(!this->compare(*it_key, key) && !this->compare(key, *it_key)) {
			this->values[key_index] = std::forward<V>(value);
			return std::make_pair(iterator(key_index, this), false);
		}
		auto index = std::distance(this->keys.begin(), it_after_key);
		auto it_after_value = this->values.begin() + index;
		this->keys.reserve(this->keys.size() + 1);
		this->values.reserve(this->values.size() + 1);
		try {
			this->keys.emplace(it_after_key, std::forward<K>(key));
			this->values.emplace(it_after_value, std::forward<V>(value));
			return std::make_pair(iterator(index, this), true);
		} catch(...) {
			this->keys.clear();
			this->values.clear();
			throw;
		}
	}

	using Base::erase;

	/// Removes the key-value-element that matches the given key from the map if it exists.
	/**
	 * Because the elements after the removed element need to be moved to close the gap and those moves might
	 * throw exceptions this operation can only provide the basic exception guarantee. If an exception occurs
	 * during removal the map will be left in an empty but valid state.
	 */
	size_t erase(const Key& key) {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			if(this->compare(key, *it)) return 0;
			erase(iterator(std::distance(this->keys.begin(), it), this));
			return 1;
		} else {
			return 0;
		}
	}

	/// Returns the number of key-value-pairs matching the given key in the map (1 or 0).
	size_t count(const Key& key) const {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			if(compare(key, *it)) return 0;
			return 1;
		} else {
			return 0;
		}
	}

	/// Returns the number of key-value-pairs matching the given key in the map (1 or 0).
	/**
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	size_t count(const K& key) const {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			if(this->compare(key, *it)) return 0;
			return 1;
		} else {
			return 0;
		}
	}

	/// Returns a reference to the value for the given key or throws an exception if the key does not exist.
	Value& at(const Key& key) {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			if(this->compare(key, *it)) std::out_of_range("Key not found.");
			return this->values[std::distance(this->keys.begin(), it)];
		} else {
			throw std::out_of_range("Key not found.");
		}
	}

	/// \brief Returns a constant reference to the value for the given key or throws an exception if the key
	/// does not exist.
	const Value& at(const Key& key) const {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			if(this->compare(key, *it)) std::out_of_range("Key not found.");
			return this->values[std::distance(this->keys.begin(), it)];
		} else {
			throw std::out_of_range("Key not found.");
		}
	}

	/// \brief Returns a reference to the value for the given key creating it using default construction if it
	/// does not exist.
	template <typename K>
	Value& operator[](K&& key) {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end())
			if(this->compare(key, *it)) it = this->keys.end();
		if(it == this->keys.end()) {
			std::tie(it, std::ignore) = insert(std::forward<K>(key), Value());
		}
		return this->values[std::distance(this->keys.begin(), it)];
	}

	/// Swaps the contents of *this with other by swapping their components.
	/**
	 * Because exceptions thrown during one of the component swaps may leave the map in an invalid or
	 * inconsistent state the operation can only provide the basic exception guarantee. If an exception is
	 * thrown by one of the component swaps both maps will be put into an empty but valid state.
	 */
	void swap(dual_container_map& other) {
		Base::swap(other);
	}
};

/// \brief Provides a non-member ADL swap by calling the member dual_container_map::swap.
template <template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(dual_container_map<Container, Key, Value, Compare>& m1,
		  dual_container_map<Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

/// \brief Implements a multimap by keeping the keys and the values in separate Container objects,
/// key-value-pairs are linked by matching indexes.
/**
 * The interface of this class resembles that of the STL multimap containers.
 * But iterators return (by value) a pair of references to the key and value instead of a reference to a pair
 * of the values of the keys and the values. This means taking the dereferenced value into auto by value still
 * yields a pair of references which can modify the value in the map.
 */
template <template <typename> class Container, typename Key, typename Value, typename Compare = std::less<>>
class dual_container_multimap
		: public dual_container_map_base<dual_container_multimap<Container, Key, Value, Compare>, Container,
										 Key, Value, Compare> {

	typedef dual_container_map_base<dual_container_multimap<Container, Key, Value, Compare>, Container, Key,
									Value, Compare>
			Base;

public:
	/// \brief Constructs a dual_container_multimap by forwarding the given parameters to both constructors of
	/// the underlying container types and default-constructing the Compare object.
	template <typename... Args>
	explicit dual_container_multimap(Args&&... args) noexcept(
			std::is_nothrow_default_constructible<Compare>::value&&
					std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: dual_container_multimap(Compare(), std::forward<Args>(args)...) {}
	/// \brief Constructs a dual_container_multimap by forwarding the given parameters to both constructors of
	/// the underlying container types and copying the Compare object.
	template <typename... Args>
	explicit dual_container_multimap(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(compare, std::forward<Args>(args)...) {}
	/// \brief Constructs a dual_container_multimap by forwarding the given parameters to both constructors of
	/// the underlying container types and moving the Compare object.
	template <typename... Args>
	explicit dual_container_multimap(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(std::move_if_noexcept(compare), std::forward<Args>(args)...) {}
	/// Copy-constructs a dual_container_multimap from other.
	dual_container_multimap(const dual_container_multimap& other) noexcept(
			std::is_nothrow_copy_constructible<Base>::value)
			: Base(other) {}
	/// Move-constructs a dual_container_multimap from other.
	dual_container_multimap(dual_container_multimap&& other) noexcept(
			std::is_nothrow_move_constructible<Base>::value)
			: Base(std::move(other)) {}
	/// Copy-assigns a dual_container_multimap from other.
	dual_container_multimap&
	operator=(const dual_container_multimap& other) noexcept(std::is_nothrow_copy_assignable<Base>::value) {
		Base::operator=(other);
		return *this;
	}
	/// Move-assigns a dual_container_multimap from other.
	dual_container_multimap&
	operator=(dual_container_multimap&& other) noexcept(std::is_nothrow_move_assignable<Base>::value) {
		Base::operator=(std::move(other));
		return *this;
	}

	/// RandomAccessIterator
	typedef typename Base::iterator iterator;
	/// Constant RandomAccessIterator
	typedef typename Base::const_iterator const_iterator;
	/// Reverse RandomAccessIterator
	typedef typename Base::reverse_iterator reverse_iterator;
	/// Reverse constant RandomAccessIterator
	typedef typename Base::const_reverse_iterator const_reverse_iterator;

	using Base::erase;

	/// Inserts a new element with the given key and value by forward-constructing them.
	/**
	 * The function reserves the required memory before modifying the underlying containers to leave the map
	 * unmodified if an exception is thrown while looking up the position for the new element or while
	 * allocating memory. However since the insertion requires moving the elements after the insertion
	 * position and those moves might throw, only the basic exception guarantee can be provided. If an
	 * exception is thrown while inserting the key or the value the map is left in an empty but valid state.
	 */
	template <typename K, typename V>
	iterator insert(K&& key, V&& value) {
		auto it_after_key = std::upper_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		auto index = std::distance(this->keys.begin(), it_after_key);
		auto it_after_value = this->values.begin() + index;
		this->keys.reserve(this->keys.size() + 1);
		this->values.reserve(this->values.size() + 1);
		try {
			this->keys.emplace(it_after_key, std::forward<K>(key));
			this->values.emplace(it_after_value, std::forward<V>(value));
			return iterator(index, this);
		} catch(...) {
			this->keys.clear();
			this->values.clear();
			throw;
		}
	}

	/// Removes all key-value-pairs that match the given key from the map.
	/**
	 * Because the elements after the removed elements need to be moved to close the gap and those moves might
	 * throw exceptions this operation can only provide the basic exception guarantee. If an exception occurs
	 * during removal the map will be left in an empty but valid state.
	 */
	size_t erase(const Key& key) {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			size_t count = 0;
			auto erase_check = [this](auto&& it, auto&& key) {
				// False positive
				// cppcheck-suppress oppositeInnerCondition
				if(it == this->keys.end()) return false;
				return !this->compare(key, *it);
			};
			while(erase_check(it, key)) {
				size_t new_index = std::distance(this->keys.begin(), it);
				it = this->keys.begin() + erase(iterator(new_index, this)).index;
				++count;
			}
			return count;
		} else {
			return 0;
		}
	}

	/// Returns the number of key-value-pairs matching the given key in the map.
	size_t count(const Key& key) const {
		auto range = equal_range(key);
		return std::distance(range.first, range.second);
	}

	/// Returns the number of key-value-pairs matching the given key in the map.
	/**
	 * The given key can be of different type than the keys in the map as long as they can be compared using
	 * the Compare template parameter of the map.
	 * This overload does only participate in overload resolution if the Compare template parameter of the map
	 * is a comparator with a is_transparent tag type.
	 */
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	size_t count(const K& key) const {
		auto range = equal_range(key);
		return std::distance(range.first, range.second);
	}

	/// Swaps the contents of *this with other by swapping their components.
	/**
	 * Because exceptions thrown during one of the component swaps may leave the map in an invalid or
	 * inconsistent state the operation can only provide the basic exception guarantee. If an exception is
	 * thrown by one of the component swaps both maps will be put into an empty but valid state.
	 */
	void swap(dual_container_multimap& other) {
		Base::swap(other);
	}
};

/// \brief Provides a non-member ADL swap by calling the member dual_container_multimap::swap.
template <template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(dual_container_multimap<Container, Key, Value, Compare>& m1,
		  dual_container_multimap<Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_DUAL_CONTAINER_MAP_HPP_ */
