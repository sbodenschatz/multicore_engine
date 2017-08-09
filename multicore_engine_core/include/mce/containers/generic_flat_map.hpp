/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/generic_flat_map.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_GENERIC_FLAT_MAP_HPP_
#define CONTAINERS_GENERIC_FLAT_MAP_HPP_

/**
 * \file
 * Defines generic map types working on sorted flat container.
 */

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace mce {
namespace containers {

/// Provides common functionality for generic_flat_map and generic_flat_multimap.
template <typename Map, template <typename> class Container, typename Key, typename Value,
		  typename Compare = std::less<>>
class generic_flat_map_base {
protected:
	/// The type of the container used to store key-value-pairs.
	typedef Container<std::pair<Key, Value>> container_t;
	/// The container storing the key-value-pairs.
	container_t values;
	/// Comparator function object for keys.
	Compare compare;

	/// Comparator function object to compare keys with keys in elements for less than.
	struct key_compare {
		/// Constructs a key_compare from the given comparator.
		explicit key_compare(const Compare& comp) : comp(comp) {}
		/// The used comparator.
		const Compare& comp;
		/// Compares the key in the given element with the given key.
		bool operator()(const std::pair<Key, Value>& a, const Key& b) const {
			return comp(a.first, b);
		}
		/// Compares the given key with the key in the given element.
		bool operator()(const Key& a, const std::pair<Key, Value>& b) const {
			return comp(a, b.first);
		}
		/// Compares the keys in the given elements.
		bool operator()(const std::pair<Key, Value>& a, const std::pair<Key, Value>& b) const {
			return comp(a.first, b.first);
		}
	};

	/// Used by implementing classes to construct a base by copying the comparator and forwarding the args.
	template <typename... Args>
	explicit generic_flat_map_base(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<container_t, Args...>::value&&
					std::is_nothrow_copy_constructible<Compare>::value)
			: values(std::forward<Args>(args)...), compare(compare) {
		std::stable_sort(values.begin(), values.end(),
						 [&compare](const auto& a, const auto& b) { return compare(a.first, b.first); });
	}
	/// Used by implementing classes to construct a base by moving the comparator and forwarding the args.
	template <typename... Args>
	explicit generic_flat_map_base(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<container_t, Args...>::value &&
			(std::is_nothrow_copy_constructible<Compare>::value ||
			 std::is_nothrow_move_constructible<Compare>::value))
			: values(std::forward<Args>(args)...), compare(std::move_if_noexcept(compare)) {
		std::stable_sort(values.begin(), values.end(),
						 [&compare](const auto& a, const auto& b) { return compare(a.first, b.first); });
	}
	/// Used by implementing classes to copy-construct.
	generic_flat_map_base(const generic_flat_map_base& other) noexcept(
			std::is_nothrow_copy_constructible<container_t>::value&&
					std::is_nothrow_copy_constructible<Compare>::value)
			: values(other.values), compare(other.compare) {}
	/// Used by implementing classes to move-construct.
	generic_flat_map_base(generic_flat_map_base&& other) noexcept(
			(std::is_nothrow_copy_constructible<container_t>::value ||
			 std::is_nothrow_move_constructible<container_t>::value) &&
			(std::is_nothrow_copy_constructible<Compare>::value ||
			 std::is_nothrow_move_constructible<Compare>::value))
			: values(std::move_if_noexcept(other.values)), compare(std::move_if_noexcept(other.compare)) {}
	/// Used by implementing classes to copy-assign.
	generic_flat_map_base& operator=(const generic_flat_map_base& other) noexcept(
			std::is_nothrow_copy_assignable<container_t>::value&&
					std::is_nothrow_copy_assignable<Compare>::value) {
		compare = other.compare;
		values.reserve(other.values.size());
		values = other.values;
		return *this;
	}
	/// Used by implementing classes to move-assign.
	generic_flat_map_base& operator=(generic_flat_map_base&& other) noexcept(
			std::is_nothrow_copy_assignable<container_t>::value&&
					std::is_nothrow_copy_assignable<Compare>::value) {
		assert(this == &other);
		compare = std::move_if_noexcept(other.compare);
		values = std::move_if_noexcept(other.values);
		return *this;
	}

public:
	/// Iterator type for generic_flat_map and generic_flat_multimap.
	/**
	* Provides access to the key-value-pairs in the container.
	* Satisfies the requirements of the RandomAccessIterator concept.
	*/
	template <typename It_Map, typename It, typename It_T>
	class iterator_ : public std::iterator<std::random_access_iterator_tag, It_T> {
		It iterator;

		explicit iterator_(It iterator) noexcept : iterator(iterator) {}

		template <typename M, typename I, typename IT>
		friend class iterator_;
		friend It_Map;
		template <typename M, template <typename> class Cont, typename K, typename V, typename Comp>
		friend class generic_flat_map_base;

	public:
		/// Disallow default-construction.
		iterator_() = delete;

		/// Copy-constructs an iterator.
		// cppcheck-suppress noExplicitConstructor
		iterator_(const typename It_Map::iterator& it) noexcept : iterator(it.iterator) {}

		/// Copy-assigns an iterator.
		iterator_& operator=(const typename It_Map::iterator& it) noexcept {
			iterator = it.iterator;
			return *this;
		}

		/// Returns a reference to the currently referenced key-value-pair.
		typename iterator_::reference operator*() const noexcept {
			return *iterator;
		}

		/// Allows member access to the currently referenced key-value-pair.
		typename iterator_::pointer operator->() const noexcept {
			return iterator.operator->();
		}

		/// Returns true if and only if the *this and it refer to the same key-value-element of the same map.
		bool operator==(const iterator_& it) const noexcept {
			return it.iterator == iterator;
		}

		/// Returns false if and only if the *this and it refer to the same key-value-element of the same map.
		bool operator!=(const iterator_& it) const noexcept {
			return !(*this == it);
		}

		/// Advances the iterator to the next element and returns the new iterator.
		iterator_& operator++() noexcept {
			++iterator;
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
			--iterator;
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
			iterator += n;
			return *this;
		}

		/// Advances the iterator n elements back.
		iterator_& operator-=(typename iterator_::difference_type n) noexcept {
			iterator -= n;
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
			return it1.iterator - it2.iterator;
		}

		/// Allows access to the key and value pair n elements ahead as a pair of references.
		typename iterator_::reference operator[](int n) noexcept {
			return iterator[n];
		}

		/// \brief Returns true if and only if the referenced element of *this is in the same map as the
		/// referenced element of other and is located before it.
		/**
		* Note that iterators from different maps compare false for <,>,>=, and <=.
		*/
		bool operator<(iterator_ other) noexcept {
			return iterator < other.iterator;
		}

		/// \brief Returns true if and only if the referenced element of *this is in the same map as the
		/// referenced element of other and is located after it.
		/**
		* Note that iterators from different maps compare false for <,>,>=, and <=.
		*/
		bool operator>(iterator_ other) noexcept {
			return iterator > other.iterator;
		}

		/// \brief Returns true if and only if the referenced element of *this is in the same map as the
		/// referenced element of other and is located before it or if both reference the same element.
		/**
		* Note that iterators from different maps compare false for <,>,>=, and <=.
		*/
		bool operator<=(iterator_ other) noexcept {
			return iterator <= other.iterator;
		}

		/// \brief Returns true if and only if the referenced element of *this is in the same map as the
		/// referenced element of other and is located after it or if both reference the same element.
		/**
		* Note that iterators from different maps compare false for <,>,>=, and <=.
		*/
		bool operator>=(iterator_ other) noexcept {
			return iterator >= other.iterator;
		}
	};

	/// RandomAccessIterator
	typedef iterator_<Map, typename container_t::iterator, std::pair<Key, Value>> iterator;
	/// Constant RandomAccessIterator
	typedef iterator_<Map, typename container_t::const_iterator, const std::pair<Key, Value>> const_iterator;

	/// Reverse RandomAccessIterator
	typedef std::reverse_iterator<iterator> reverse_iterator;
	/// Reverse constant RandomAccessIterator
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	/// Removes all key-value-elements from the map and thus makes it empty.
	void clear() noexcept {
		values.clear();
	}

	/// Removes the key-value-element referenced by pos from the map.
	iterator erase(iterator pos) {
		return erase(const_iterator(pos));
	}

	/// Removes the key-value-element referenced by pos from the map.
	iterator erase(const_iterator pos) {
		auto it = values.erase(pos.iterator);
		return iterator(it);
	}

	/// Swaps the contents of *this with other by swapping their components.
	void swap(generic_flat_map_base& other) {
		using std::swap;
		swap(compare, other.compare);
		swap(values, other.values);
	}

	/// \brief Looks up the element(s) with the given key and returns an iterator to the first such element or
	/// an past-end-iterator if no such element exists.
	iterator find(const Key& key) {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		if(it != values.end())
			if(compare(key, it->first)) it = values.end();
		return iterator(it);
	}

	/// \brief Looks up the element(s) with the given key and returns a const_iterator to the first such
	/// element or an past-end-iterator if no such element exists.
	const_iterator find(const Key& key) const {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		if(it != values.end())
			if(compare(key, it->first)) it = values.end();
		return const_iterator(it);
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
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		if(it != values.end())
			if(comp(key, it->first)) it = values.end();
		return iterator(it);
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
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		if(it != values.end())
			if(comp(key, it->first)) it = values.end();
		return const_iterator(it);
	}

	/// \brief Returns an iterator referencing the first element in the map whose key is not less than the
	/// given key or a past-end-iterator if no such element exist.
	iterator lower_bound(const Key& key) {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		return iterator(it);
	}

	/// \brief Returns a const_iterator referencing the first element in the map whose key is not less than
	/// the given key or a past-end-iterator if no such element exist.
	const_iterator lower_bound(const Key& key) const {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		return const_iterator(it);
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
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		return iterator(it);
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
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		return const_iterator(it);
	}

	/// \brief Returns an iterator referencing the first element in the map whose key is greater than the
	/// given key or a past-end-iterator if no such element exist.
	iterator upper_bound(const Key& key) {
		key_compare comp(compare);
		auto it = std::upper_bound(values.begin(), values.end(), key, comp);
		return iterator(it);
	}

	/// \brief Returns a const_iterator referencing the first element in the map whose key is greater than the
	/// given key or a past-end-iterator if no such element exist.
	const_iterator upper_bound(const Key& key) const {
		key_compare comp(compare);
		auto it = std::upper_bound(values.begin(), values.end(), key, comp);
		return const_iterator(it);
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
		key_compare comp(compare);
		auto it = std::upper_bound(values.begin(), values.end(), key, comp);
		return iterator(it);
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
		key_compare comp(compare);
		auto it = std::upper_bound(values.begin(), values.end(), key, comp);
		return const_iterator(it);
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
		return values.empty();
	}

	/// Returns the number of elements (key-value-pairs) in the map.
	size_t size() const noexcept {
		return values.size();
	}

	/// Returns an iterator to the first key-value-pair in the map or a past-end-iterator if the map is empty.
	iterator begin() noexcept {
		return iterator(values.begin());
	}

	/// \brief Returns a const_iterator to the first key-value-pair in the map or a past-end-iterator if the
	/// map is empty.
	const_iterator begin() const noexcept {
		return const_iterator(values.begin());
	}

	/// \brief Returns a const_iterator to the first key-value-pair in the map or a past-end-iterator if the
	/// map is empty.
	const_iterator cbegin() const noexcept {
		return const_iterator(values.cbegin());
	}

	/// Returns a past-end-iterator for the map.
	iterator end() noexcept {
		return iterator(values.end());
	}

	/// Returns a constant past-end-iterator for the map.
	const_iterator end() const noexcept {
		return const_iterator(values.end());
	}

	/// Returns a constant past-end-iterator for the map.
	const_iterator cend() const noexcept {
		return const_iterator(values.end());
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

	/// Restores the correct ordering of the elements after the keys of elements were modified.
	void resort() {
		std::stable_sort(values.begin(), values.end(),
						 [this](const auto& a, const auto& b) { return compare(a.first, b.first); });
	}

	/// Compares *this and other for equality of the key-value-pairs stored in them.
	bool operator==(const generic_flat_map_base& other) const {
		return values == other.values;
	}

	/// Compares *this and other for inequality of the key-value-pairs stored in them.
	bool operator!=(const generic_flat_map_base& other) const {
		return !(*this == other);
	}
};

/// \brief Provides a non-member ADL swap by calling the member generic_flat_map_base::swap.
template <typename Map, template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(generic_flat_map_base<Map, Container, Key, Value, Compare>& m1,
		  generic_flat_map_base<Map, Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

/// \brief Implements a map by keeping the key and value pairs in sorted order in a linear Container.
/**
 * The interface of this class resembles that of the STL map containers.
 * But iterators return a std::pair<Key,Value> instead of std::pair<const Key, Value> because the latter one
 * can't be portably combined with reallocating containers. (see
 * http://stackoverflow.com/questions/14272141/is-casting-stdpairt1-t2-const-to-stdpairt1-const-t2-const-safe
 * for explanation.)
 *
 * If the Key is modified by the user, the resort() member function has to be called to restore the invariants
 * of the map.
 */
template <template <typename> class Container, typename Key, typename Value, typename Compare = std::less<>>
class generic_flat_map : public generic_flat_map_base<generic_flat_map<Container, Key, Value, Compare>,
													  Container, Key, Value, Compare> {

	typedef generic_flat_map_base<generic_flat_map<Container, Key, Value, Compare>, Container, Key, Value,
								  Compare>
			Base;
	typedef typename Base::key_compare key_compare;

public:
	/// \brief Constructs a generic_flat_map by forwarding the given parameters to the constructor of the
	/// underlying container type and default-constructing the Compare object.
	template <typename... Args>
	explicit generic_flat_map(Args&&... args) noexcept(
			std::is_nothrow_default_constructible<Compare>::value&&
					std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: generic_flat_map(Compare(), std::forward<Args>(args)...) {}
	/// \brief Constructs a generic_flat_map by forwarding the given parameters to the constructor of the
	/// underlying container type and copying the Compare object.
	template <typename... Args>
	explicit generic_flat_map(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(compare, std::forward<Args>(args)...) {}
	/// \brief Constructs a generic_flat_map by forwarding the given parameters to the constructor of the
	/// underlying container type and moving the Compare object.
	template <typename... Args>
	explicit generic_flat_map(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(std::move_if_noexcept(compare), std::forward<Args>(args)...) {}
	/// Copy-constructs a generic_flat_map from other.
	generic_flat_map(const generic_flat_map& other) noexcept(std::is_nothrow_copy_constructible<Base>::value)
			: Base(other) {}
	/// Move-constructs a generic_flat_map from other.
	generic_flat_map(generic_flat_map&& other) noexcept(std::is_nothrow_move_constructible<Base>::value)
			: Base(std::move(other)) {}
	/// Copy-assigns a generic_flat_map from other.
	generic_flat_map&
	operator=(const generic_flat_map& other) noexcept(std::is_nothrow_copy_assignable<Base>::value) {
		Base::operator=(other);
		return *this;
	}
	/// Move-assigns a generic_flat_map from other.
	generic_flat_map&
	operator=(generic_flat_map&& other) noexcept(std::is_nothrow_move_assignable<Base>::value) {
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
	*/
	template <typename K, typename V>
	std::pair<iterator, bool> insert(K&& key, V&& value) {
		key_compare comp(this->compare);
		auto it_after = std::upper_bound(this->values.begin(), this->values.end(), key, comp);
		auto it = it_after;
		if(it != this->values.begin()) --it;
		if(it != this->values.end() && !comp(*it, key) && !comp(key, *it))
			return std::make_pair(iterator(it), false);
		auto pos = std::distance(this->values.begin(), it_after);
		this->values.reserve(this->values.size() + 1);
		it = this->values.emplace(this->values.begin() + pos, std::forward<K>(key), std::forward<V>(value));
		return std::make_pair(iterator(it), true);
	}

	/// Assigns the given value to the given key or inserts it for the given key if the key doesn't exist.
	/**
	* The function returns an iterator to the inserted element and true if the element was new or an
	* iterator to the assigned element and false otherwise.
	*/
	template <typename K, typename V>
	std::pair<iterator, bool> insert_or_assign(K&& key, V&& value) {
		key_compare comp(this->compare);
		auto it_after = std::upper_bound(this->values.begin(), this->values.end(), key, comp);
		auto it = it_after;
		if(it != this->values.begin()) --it;
		if(it != this->values.end() && !comp(*it, key) && !comp(key, *it)) {
			it->second = std::forward<V>(value);
			return std::make_pair(iterator(it), false);
		}
		auto pos = std::distance(this->values.begin(), it_after);
		this->values.reserve(this->values.size() + 1);
		it = this->values.emplace(this->values.begin() + pos, std::forward<K>(key), std::forward<V>(value));
		return std::make_pair(iterator(it), true);
	}

	using Base::erase;

	/// Removes the key-value-element that matches the given key from the map if it exists.
	size_t erase(const Key& key) {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) return 0;
			erase(iterator(it));
			return 1;
		} else {
			return 0;
		}
	}

	/// Returns the number of key-value-pairs matching the given key in the map (1 or 0).
	size_t count(const Key& key) const {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) return 0;
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
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) return 0;
			return 1;
		} else {
			return 0;
		}
	}

	/// Returns a reference to the value for the given key or throws an exception if the key does not exist.
	Value& at(const Key& key) {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) std::out_of_range("Key not found.");
			return it->second;
		} else {
			throw std::out_of_range("Key not found.");
		}
	}

	/// \brief Returns a constant reference to the value for the given key or throws an exception if the key
	/// does not exist.
	const Value& at(const Key& key) const {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) std::out_of_range("Key not found.");
			return it->second;
		} else {
			throw std::out_of_range("Key not found.");
		}
	}

	/// \brief Returns a reference to the value for the given key creating it using default construction if it
	/// does not exist.
	template <typename K>
	Value& operator[](K&& key) {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end())
			if(comp(key, *it)) it = this->values.end();
		if(it == this->values.end()) {
			it = insert(std::forward<K>(key), Value()).first.iterator;
		}
		return it->second;
	}

	/// Swaps the contents of *this with other by swapping their components.
	void swap(generic_flat_map& other) {
		Base::swap(other);
	}
};

/// \brief Provides a non-member ADL swap by calling the member generic_flat_map::swap.
template <template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(generic_flat_map<Container, Key, Value, Compare>& m1,
		  generic_flat_map<Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

/// \brief Implements a multimap by keeping the key and value pairs in sorted order in a linear Container.
/**
 * The interface of this class resembles that of the STL multimap containers.
 * But iterators return a std::pair<Key,Value> instead of std::pair<const Key, Value> because the latter one
 * can't be portably combined with reallocating containers. (see
 * http://stackoverflow.com/questions/14272141/is-casting-stdpairt1-t2-const-to-stdpairt1-const-t2-const-safe
 * for explanation.)
 *
 * If the Key is modified by the user, the resort() member function has to be called to restore the invariants
 * of the map.
 */
template <template <typename> class Container, typename Key, typename Value, typename Compare = std::less<>>
class generic_flat_multimap
		: public generic_flat_map_base<generic_flat_multimap<Container, Key, Value, Compare>, Container, Key,
									   Value, Compare> {

	typedef generic_flat_map_base<generic_flat_multimap<Container, Key, Value, Compare>, Container, Key,
								  Value, Compare>
			Base;

public:
	/// \brief Constructs a dual_container_multimap by forwarding the given parameters to the constructor of
	/// the underlying container type and default-constructing the Compare object.
	template <typename... Args>
	explicit generic_flat_multimap(Args&&... args) noexcept(
			std::is_nothrow_default_constructible<Compare>::value&&
					std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: generic_flat_multimap(Compare(), std::forward<Args>(args)...) {}

	/// \brief Constructs a dual_container_multimap by forwarding the given parameters to the constructor of
	/// the underlying container type and copying the Compare object.
	template <typename... Args>
	explicit generic_flat_multimap(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(compare, std::forward<Args>(args)...) {}

	/// \brief Constructs a dual_container_multimap by forwarding the given parameters to the constructor of
	/// the underlying container type and moving the Compare object.
	template <typename... Args>
	explicit generic_flat_multimap(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(std::move_if_noexcept(compare), std::forward<Args>(args)...) {}
	/// Copy-constructs a generic_flat_multimap from other.
	generic_flat_multimap(const generic_flat_multimap& other) noexcept(
			std::is_nothrow_copy_constructible<Base>::value)
			: Base(other) {}
	/// Move-constructs a generic_flat_multimap from other.
	generic_flat_multimap(generic_flat_multimap&& other) noexcept(
			std::is_nothrow_move_constructible<Base>::value)
			: Base(std::move(other)) {}
	/// Copy-assigns a generic_flat_multimap from other.
	generic_flat_multimap&
	operator=(const generic_flat_multimap& other) noexcept(std::is_nothrow_copy_assignable<Base>::value) {
		Base::operator=(other);
		return *this;
	}
	/// Move-assigns a generic_flat_multimap from other.
	generic_flat_multimap&
	operator=(generic_flat_multimap&& other) noexcept(std::is_nothrow_move_assignable<Base>::value) {
		Base::operator=(std::move(other));
		return *this;
	}

	/// Comparator function to compare keys in elements with keys.
	typedef typename Base::key_compare key_compare;
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
	template <typename K, typename V>
	iterator insert(K&& key, V&& value) {
		key_compare comp(this->compare);
		auto it_after = std::upper_bound(this->values.begin(), this->values.end(), key, comp);
		auto pos = std::distance(this->values.begin(), it_after);
		this->values.reserve(this->values.size() + 1);
		auto it = this->values.emplace(this->values.begin() + pos, std::forward<K>(key),
									   std::forward<V>(value));
		return iterator(it);
	}

	/// Removes all key-value-pairs that match the given key from the map.
	size_t erase(const Key& key) {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			size_t count = 0;
			auto erase_check = [this, &comp](auto&& it, auto&& key) {
				// False positive
				// cppcheck-suppress oppositeInnerCondition
				if(it == this->values.end()) return false;
				return !comp(key, *it);
			};
			while(erase_check(it, key)) {
				it = erase(iterator(it)).iterator;
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
	void swap(generic_flat_multimap& other) {
		Base::swap(other);
	}
};

/// \brief Provides a non-member ADL swap by calling the member generic_flat_multimap::swap.
template <template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(generic_flat_multimap<Container, Key, Value, Compare>& m1,
		  generic_flat_multimap<Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_GENERIC_FLAT_MAP_HPP_ */
