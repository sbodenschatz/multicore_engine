/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/dual_container_map.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_DUAL_CONTAINER_MAP_HPP_
#define CONTAINERS_DUAL_CONTAINER_MAP_HPP_

#include <utility>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <tuple>

namespace mce {
namespace containers {
namespace detail {
template <typename Map, template <typename> class Container, typename Key, typename Value,
		  typename Compare = std::less<>>
class dual_container_map_base {
protected:
	Container<Key> keys;
	Container<Value> values;
	Compare compare;

public:
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

	template <typename T>
	struct member_access_wrapper {
		T value;
		template <typename... Args>
		member_access_wrapper(Args&&... args)
				: value{std::forward<Args>(args)...} {}
		T* operator->() noexcept {
			return &value;
		}
	};

	template <typename It_Map, typename It_Key, typename It_Value>
	class iterator_ : public std::iterator<std::bidirectional_iterator_tag, std::pair<It_Key&, It_Value&>> {
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

		std::pair<It_Key&, It_Value&> operator*() const noexcept {
			return std::make_pair(std::ref(map->keys[index]), std::ref(map->values[index]));
		}
		member_access_wrapper<std::pair<It_Key&, It_Value&>> operator->() const noexcept {
			return member_access_wrapper<std::pair<It_Key&, It_Value&>>(operator*());
		}
		bool operator==(const iterator_& it) const noexcept {
			return it.index == index && it.map == map;
		}
		bool operator!=(const iterator_& it) const noexcept {
			return !(*this == it);
		}
		iterator_& operator++() noexcept {
			++index;
			return *this;
		}
		iterator_ operator++(int) noexcept {
			auto it = *this;
			this->operator++();
			return it;
		}
		iterator_& operator--() noexcept {
			--index;
			return *this;
		}
		iterator_ operator--(int) noexcept {
			auto it = *this;
			this->operator--();
			return it;
		}
		iterator_& operator+=(typename iterator_::difference_type n) noexcept {
			index += n;
			return *this;
		}
		iterator_& operator-=(typename iterator_::difference_type n) noexcept {
			index -= n;
			return *this;
		}
		friend iterator_ operator+(iterator_ it, typename iterator_::difference_type n) noexcept {
			return it += n;
		}
		friend iterator_ operator+(typename iterator_::difference_type n, iterator_ it) noexcept {
			return it += n;
		}
		friend iterator_ operator-(iterator_ it, typename iterator_::difference_type n) noexcept {
			return it -= n;
		}
		friend typename iterator_::difference_type operator-(iterator_ it1, iterator_ it2) noexcept {
			return it1.index - it2.index;
		}
		std::pair<It_Key&, It_Value&> operator[](int n) noexcept {
			return *((*this) + n);
		}
		bool operator<(iterator_ other) noexcept {
			return (map == other.map) && (index < other.index);
		}
		bool operator>(iterator_ other) noexcept {
			return (map == other.map) && (index > other.index);
		}
		bool operator<=(iterator_ other) noexcept {
			return (map == other.map) && (index <= other.index);
		}
		bool operator>=(iterator_ other) noexcept {
			return (map == other.map) && (index >= other.index);
		}
	};

	typedef iterator_<Map, const Key, Value> iterator;
	typedef iterator_<const Map, const Key, const Value> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	void clear() noexcept {
		keys.clear();
		values.clear();
	}

	iterator erase(iterator pos) {
		return erase(const_iterator(pos));
	}
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

	iterator find(const Key& key) {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		if(it != keys.end())
			if(compare(key, *it)) it = keys.end();
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}
	const_iterator find(const Key& key) const {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		if(it != keys.end())
			if(compare(key, *it)) it = keys.end();
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator find(const K& key) {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		if(it != keys.end())
			if(compare(key, *it)) it = keys.end();
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator find(const K& key) const {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		if(it != keys.end())
			if(compare(key, *it)) it = keys.end();
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}
	iterator lower_bound(const Key& key) {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}
	const_iterator lower_bound(const Key& key) const {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator lower_bound(const K& key) {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator lower_bound(const K& key) const {
		auto it = std::lower_bound(keys.begin(), keys.end(), key, compare);
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}
	iterator upper_bound(const Key& key) {
		auto it = std::upper_bound(keys.begin(), keys.end(), key, compare);
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}
	const_iterator upper_bound(const Key& key) const {
		auto it = std::upper_bound(keys.begin(), keys.end(), key, compare);
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator upper_bound(const K& key) {
		auto it = std::upper_bound(keys.begin(), keys.end(), key, compare);
		return iterator(std::distance(keys.begin(), it), static_cast<Map*>(this));
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator upper_bound(const K& key) const {
		auto it = std::upper_bound(keys.begin(), keys.end(), key, compare);
		return const_iterator(std::distance(keys.begin(), it), static_cast<const Map*>(this));
	}
	std::pair<iterator, iterator> equal_range(const Key& key) {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}
	std::pair<const_iterator, const_iterator> equal_range(const Key& key) const {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}
	template <typename K>
	std::pair<iterator, iterator> equal_range(const K& key) {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}
	template <typename K>
	std::pair<const_iterator, const_iterator> equal_range(const K& key) const {
		return std::make_pair(lower_bound(key), upper_bound(key));
	}

	bool empty() const noexcept {
		return keys.empty();
	}

	size_t size() const noexcept {
		return keys.size();
	}

	iterator begin() noexcept {
		return iterator(0, static_cast<Map*>(this));
	}
	const_iterator begin() const noexcept {
		return const_iterator(0, static_cast<const Map*>(this));
	}
	const_iterator cbegin() const noexcept {
		return const_iterator(0, static_cast<const Map*>(this));
	}

	iterator end() noexcept {
		return iterator(keys.size(), static_cast<Map*>(this));
	}
	const_iterator end() const noexcept {
		return const_iterator(keys.size(), static_cast<const Map*>(this));
	}
	const_iterator cend() const noexcept {
		return const_iterator(keys.size(), static_cast<const Map*>(this));
	}

	reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}
	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}
	const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(cend());
	}

	reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}
	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}
	const_reverse_iterator crend() const noexcept {
		return const_reverse_iterator(cbegin());
	}
};

template <typename Map, template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(dual_container_map_base<Map, Container, Key, Value, Compare>& m1,
		  dual_container_map_base<Map, Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}
} // namespace detail

// Interface resembling that of the STL map containers
// But iterators return (by value) a pair of references to the key and value instead of a reference to a pair
// of the values of the keys and the values. This means taking the dereferenced value into auto by value still
// yields a pair of references which can modify the value in the map.
template <template <typename> class Container, typename Key, typename Value, typename Compare = std::less<>>
class dual_container_map
		: public detail::dual_container_map_base<dual_container_map<Container, Key, Value, Compare>,
												 Container, Key, Value, Compare> {

	typedef detail::dual_container_map_base<dual_container_map<Container, Key, Value, Compare>, Container,
											Key, Value, Compare> Base;

public:
	template <typename... Args>
	dual_container_map(Args&&... args) noexcept(
			std::is_nothrow_default_constructible<Compare>::value&&
					std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: dual_container_map(Compare(), std::forward<Args>(args)...) {}
	template <typename... Args>
	explicit dual_container_map(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(compare, std::forward<Args>(args)...) {}
	template <typename... Args>
	explicit dual_container_map(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(std::move_if_noexcept(compare), std::forward<Args>(args)...) {}
	dual_container_map(const dual_container_map& other) noexcept(
			std::is_nothrow_copy_constructible<Base>::value)
			: Base(other) {}
	dual_container_map(dual_container_map&& other) noexcept(std::is_nothrow_move_constructible<Base>::value)
			: Base(std::move(other)) {}
	dual_container_map&
	operator=(const dual_container_map& other) noexcept(std::is_nothrow_copy_assignable<Base>::value) {
		Base::operator=(other);
		return *this;
	}
	dual_container_map&
	operator=(dual_container_map&& other) noexcept(std::is_nothrow_move_assignable<Base>::value) {
		Base::operator=(std::move(other));
		return *this;
	}

	typedef typename Base::iterator iterator;
	typedef typename Base::const_iterator const_iterator;
	typedef typename Base::reverse_iterator reverse_iterator;
	typedef typename Base::const_reverse_iterator const_reverse_iterator;

	template <typename K, typename V>
	std::pair<iterator, bool> insert(K&& key, V&& value) {
		try {
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
			this->keys.emplace(it_after_key, std::forward<K>(key));
			this->values.emplace(it_after_value, std::forward<V>(value));
			return std::make_pair(iterator(index, this), true);
		} catch(...) {
			this->keys.clear();
			this->values.clear();
			throw;
		}
	}

	template <typename K, typename V>
	std::pair<iterator, bool> insert_or_assign(K&& key, V&& value) {
		try {
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
	size_t count(const Key& key) const {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			if(compare(key, *it)) return 0;
			return 1;
		} else {
			return 0;
		}
	}
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

	Value& at(const Key& key) {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			if(this->compare(key, *it)) std::out_of_range("Key not found.");
			return this->values[std::distance(this->keys.begin(), it)];
		} else {
			throw std::out_of_range("Key not found.");
		}
	}
	const Value& at(const Key& key) const {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			if(this->compare(key, *it)) std::out_of_range("Key not found.");
			return this->values[std::distance(this->keys.begin(), it)];
		} else {
			throw std::out_of_range("Key not found.");
		}
	}
	template <typename K>
	Value& operator[](K&& key) {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end())
			if(this->compare(key, *it)) it = this->keys.end();
		if(it == this->keys.end()) { std::tie(it, std::ignore) = insert(std::forward<K>(key), Value()); }
		return this->values[std::distance(this->keys.begin(), it)];
	}

	void swap(dual_container_map& other) {
		Base::swap(other);
	}
};

template <template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(dual_container_map<Container, Key, Value, Compare>& m1,
		  dual_container_map<Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

// Interface resembling that of the STL map containers
// But iterators return (by value) a pair of references to the key and value instead of a reference to a pair
// of the values of the keys and the values. This means taking the dereferenced value into auto by value still
// yields a pair of references which can modify the value in the map.
template <template <typename> class Container, typename Key, typename Value, typename Compare = std::less<>>
class dual_container_multimap
		: public detail::dual_container_map_base<dual_container_multimap<Container, Key, Value, Compare>,
												 Container, Key, Value, Compare> {

	typedef detail::dual_container_map_base<dual_container_multimap<Container, Key, Value, Compare>,
											Container, Key, Value, Compare> Base;

public:
	template <typename... Args>
	dual_container_multimap(Args&&... args) noexcept(
			std::is_nothrow_default_constructible<Compare>::value&&
					std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: dual_container_multimap(Compare(), std::forward<Args>(args)...) {}
	template <typename... Args>
	explicit dual_container_multimap(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(compare, std::forward<Args>(args)...) {}
	template <typename... Args>
	explicit dual_container_multimap(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(std::move_if_noexcept(compare), std::forward<Args>(args)...) {}
	dual_container_multimap(const dual_container_multimap& other) noexcept(
			std::is_nothrow_copy_constructible<Base>::value)
			: Base(other) {}
	dual_container_multimap(dual_container_multimap&& other) noexcept(
			std::is_nothrow_move_constructible<Base>::value)
			: Base(std::move(other)) {}
	dual_container_multimap&
	operator=(const dual_container_multimap& other) noexcept(std::is_nothrow_copy_assignable<Base>::value) {
		Base::operator=(other);
		return *this;
	}
	dual_container_multimap&
	operator=(dual_container_multimap&& other) noexcept(std::is_nothrow_move_assignable<Base>::value) {
		Base::operator=(std::move(other));
		return *this;
	}

	typedef typename Base::iterator iterator;
	typedef typename Base::const_iterator const_iterator;
	typedef typename Base::reverse_iterator reverse_iterator;
	typedef typename Base::const_reverse_iterator const_reverse_iterator;

	using Base::erase;
	template <typename K, typename V>
	iterator insert(K&& key, V&& value) {
		try {
			auto it_after_key = std::upper_bound(this->keys.begin(), this->keys.end(), key, this->compare);
			auto index = std::distance(this->keys.begin(), it_after_key);
			auto it_after_value = this->values.begin() + index;
			this->keys.reserve(this->keys.size() + 1);
			this->values.reserve(this->values.size() + 1);
			this->keys.emplace(it_after_key, std::forward<K>(key));
			this->values.emplace(it_after_value, std::forward<V>(value));
			return iterator(index, this);
		} catch(...) {
			this->keys.clear();
			this->values.clear();
			throw;
		}
	}

	size_t erase(const Key& key) {
		auto it = std::lower_bound(this->keys.begin(), this->keys.end(), key, this->compare);
		if(it != this->keys.end()) {
			size_t count = 0;
			auto erase_check = [this](auto&& it, auto&& key) {
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
	size_t count(const Key& key) const {
		auto range = equal_range(key);
		return std::distance(range.first, range.second);
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	size_t count(const K& key) const {
		auto range = equal_range(key);
		return std::distance(range.first, range.second);
	}
	void swap(dual_container_multimap& other) {
		Base::swap(other);
	}
};

template <template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(dual_container_multimap<Container, Key, Value, Compare>& m1,
		  dual_container_multimap<Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_DUAL_CONTAINER_MAP_HPP_ */
