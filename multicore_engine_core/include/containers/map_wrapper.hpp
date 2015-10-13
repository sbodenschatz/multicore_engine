/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/map_wrapper.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_MAP_WRAPPER_HPP_
#define CONTAINERS_MAP_WRAPPER_HPP_

#include <utility>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <cassert>
#include <iterator>

namespace mce {
namespace containers {

// Interface resembling that of the STL map containers
template <template <typename> class Container, typename Key, typename Value,
		  typename Compare = std::less<Key>>
class map_wrapper {
	Container<Key> keys;
	Container<Value> values;
	Compare compare;

public:
	template <typename... Args>
	explicit map_wrapper(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_default_constructible<Container<Key>>::value&&
					std::is_nothrow_default_constructible<Container<Value>>::value&&
							std::is_nothrow_default_constructible<Compare>::value&&
									std::is_nothrow_copy_constructible<Compare>::value)
			: keys(std::forward<Args>(args)...), values(std::forward<Args>(args)...), compare(compare) {}
	template <typename... Args>
	explicit map_wrapper(Compare&& compare = Compare(), Args&&... args) noexcept(
			std::is_nothrow_default_constructible<Container<Key>>::value&&
					std::is_nothrow_default_constructible<Container<Value>>::value&&
							std::is_nothrow_default_constructible<Compare>::value &&
			(std::is_nothrow_copy_constructible<Compare>::value ||
			 std::is_nothrow_move_constructible<Compare>::value))
			: keys(std::forward<Args>(args)...), values(std::forward<Args>(args)...),
			  compare(std::move_if_noexcept(compare)) {}
	map_wrapper(const map_wrapper& other) noexcept(
			std::is_nothrow_copy_constructible<Container<Key>>::value&& std::is_nothrow_copy_constructible<
					Container<Value>>::value&& std::is_nothrow_copy_constructible<Compare>::value)
			: keys(other.keys), values(other.values), compare(other.compare) {}
	map_wrapper(map_wrapper&& other) noexcept((std::is_nothrow_copy_constructible<Container<Key>>::value ||
											   std::is_nothrow_move_constructible<Container<Key>>::value) &&
											  (std::is_nothrow_copy_constructible<Container<Value>>::value ||
											   std::is_nothrow_move_constructible<Container<Value>>::value) &&
											  (std::is_nothrow_copy_constructible<Compare>::value ||
											   std::is_nothrow_move_constructible<Compare>::value))
			: keys(std::move_if_noexcept(other.keys)), values(std::move_if_noexcept(other.values)),
			  compare(std::move_if_noexcept(other.compare)) {}
	map_wrapper& operator=(const map_wrapper& other) noexcept(
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
	map_wrapper& operator=(map_wrapper&& other) noexcept(
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

	class iterator {
		iterator(size_t) {}
		// TODO Implement
	};
	class const_iterator {
		const_iterator(size_t) {}
		// TODO Implement
	};
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	void clear() noexcept {
		keys.clear();
		values.clear();
	}

	template <typename K, typename V>
	std::pair<iterator, bool> insert(const K&& key, const V&& value) {
		try {
			auto it_key = std::lower_bound(keys.begin(), keys.end(), key, compare);
			if(it_key == keys.end()) {
				auto index = keys.size();
				keys.reserve(keys.size() + 1);
				values.reserve(values.size() + 1);
				keys.emplace_back(std::forward<K>(key));
				values.emplace_back(std::forward<V>(value));
				return std::make_pair(iterator(index), true);
			}
			auto index = std::distance(keys.begin(), it_key);
			if(!compare(*it_key, key) && !compare(key, *it_key))
				return std::make_pair(iterator(index), false);
			auto it_value = values.begin() + index;
			keys.reserve(keys.size() + 1);
			values.reserve(values.size() + 1);
			keys.emplace(it_key, std::forward<K>(key));
			values.emplace(it_value, std::forward<V>(value));
			return std::make_pair(iterator(index), true);
		} catch(...) {
			keys.clear();
			values.clear();
			throw;
		}
	}
	template <typename K, typename V>
	std::pair<iterator, bool> insert_or_assign(const K&& key, const V&& value) {
		try {
			auto it_key = std::lower_bound(keys.begin(), keys.end(), key, compare);
			if(it_key == keys.end()) {
				auto index = keys.size();
				keys.reserve(keys.size() + 1);
				values.reserve(values.size() + 1);
				keys.emplace_back(std::forward<K>(key));
				values.emplace_back(std::forward<V>(value));
				return std::make_pair(iterator(index), true);
			}
			auto index = std::distance(keys.begin(), it_key);
			if(!compare(*it_key, key) && !compare(key, *it_key)) {
				values[index] = std::forward<V>(value);
				return std::make_pair(iterator(index), false);
			}
			auto it_value = values.begin() + index;
			keys.reserve(keys.size() + 1);
			values.reserve(values.size() + 1);
			keys.emplace(it_key, std::forward<K>(key));
			values.emplace(it_value, std::forward<V>(value));
			return std::make_pair(iterator(index), true);
		} catch(...) {
			keys.clear();
			values.clear();
			throw;
		}
	}
	iterator erase(iterator pos);
	iterator erase(const_iterator pos);
	size_t erase(const Key& key);
	void swap(map_wrapper& other);
	size_t count(const Key& key) const;
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	size_t count(const K& x) const;
	iterator find(const Key& key);
	const_iterator find(const Key& key) const;
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator find(const K& x);
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator find(const K& x) const;
	iterator lower_bound(const Key& key);
	const_iterator lower_bound(const Key& key) const;
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator lower_bound(const K& x);
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator lower_bound(const K& x) const;
	iterator upper_bound(const Key& key);
	const_iterator upper_bound(const Key& key) const;
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator upper_bound(const K& x);
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator upper_bound(const K& x) const;
	std::pair<iterator, iterator> equal_range(const Key& key);
	std::pair<const_iterator, const_iterator> equal_range(const Key& key) const;
	template <typename K>
	std::pair<iterator, iterator> equal_range(const K& x);
	template <typename K>
	std::pair<const_iterator, const_iterator> equal_range(const K& x) const;

	bool empty() const noexcept {
		return keys.empty();
	}

	size_t size() const noexcept {
		return keys.size();
	}

	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;

	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

	iterator rbegin() noexcept;
	const_iterator rbegin() const noexcept;
	const_iterator crbegin() const noexcept;

	iterator rend() noexcept;
	const_iterator rend() const noexcept;
	const_iterator crend() const noexcept;
};

template <template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(map_wrapper<Container, Key, Value, Compare>& m1, map_wrapper<Container, Key, Value, Compare>& m2);

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_MAP_WRAPPER_HPP_ */
