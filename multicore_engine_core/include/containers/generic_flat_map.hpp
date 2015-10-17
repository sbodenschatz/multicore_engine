/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/containers/generic_flat_map.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef CONTAINERS_GENERIC_FLAT_MAP_HPP_
#define CONTAINERS_GENERIC_FLAT_MAP_HPP_

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
class generic_flat_map_base {
protected:
	typedef Container<std::pair<Key, Value>> container_t;
	container_t values;
	Compare compare;

	struct key_compare {
		key_compare(Compare& comp) : comp(comp) {}
		Compare& comp;
		bool operator()(const std::pair<Key, Value>& a, const Key& b) const {
			return comp(a.first, b);
		}
		bool operator()(const Key& a, const std::pair<Key, Value>& b) const {
			return comp(a, b.first);
		}
		bool operator()(const std::pair<Key, Value>& a, const std::pair<Key, Value>& b) const {
			return comp(a.first, b.first);
		}
	};

public:
	template <typename... Args>
	explicit generic_flat_map_base(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<container_t, Args...>::value&&
					std::is_nothrow_copy_constructible<Compare>::value)
			: values(std::forward<Args>(args)...), compare(compare) {
		std::stable_sort(values.begin(), values.end(),
						 [&compare](const auto& a, const auto& b) { return compare(a.first, b.first); });
	}
	template <typename... Args>
	explicit generic_flat_map_base(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<container_t, Args...>::value &&
			(std::is_nothrow_copy_constructible<Compare>::value ||
			 std::is_nothrow_move_constructible<Compare>::value))
			: values(std::forward<Args>(args)...), compare(std::move_if_noexcept(compare)) {
		std::stable_sort(values.begin(), values.end(),
						 [&compare](const auto& a, const auto& b) { return compare(a.first, b.first); });
	}
	generic_flat_map_base(const generic_flat_map_base& other) noexcept(std::is_nothrow_copy_constructible<
			container_t>::value&& std::is_nothrow_copy_constructible<Compare>::value)
			: values(other.values), compare(other.compare) {}
	generic_flat_map_base(generic_flat_map_base&& other) noexcept(
			(std::is_nothrow_copy_constructible<container_t>::value ||
			 std::is_nothrow_move_constructible<container_t>::value) &&
			(std::is_nothrow_copy_constructible<Compare>::value ||
			 std::is_nothrow_move_constructible<Compare>::value))
			: values(std::move_if_noexcept(other.values)), compare(std::move_if_noexcept(other.compare)) {}
	generic_flat_map_base& operator=(const generic_flat_map_base& other) noexcept(
			std::is_nothrow_copy_assignable<container_t>::value&&
					std::is_nothrow_copy_assignable<Compare>::value) {
		compare = other.compare;
		values.reserve(other.values.size());
		values = other.values;
		return *this;
	}
	generic_flat_map_base& operator=(generic_flat_map_base&& other) noexcept(std::is_nothrow_copy_assignable<
			container_t>::value&& std::is_nothrow_copy_assignable<Compare>::value) {
		assert(this == &other);
		compare = std::move_if_noexcept(other.compare);
		values = std::move_if_noexcept(other.values);
		return *this;
	}

	template <typename It_Map, typename It, typename It_Key, typename It_Value>
	class iterator_ : public std::iterator<std::bidirectional_iterator_tag, std::pair<It_Key, It_Value>> {
		It iterator;

		iterator_(It iterator) noexcept : iterator(iterator) {}

	public:
		template <typename M, typename I, typename IK, typename IV>
		friend class iterator_;
		friend It_Map;
		template <typename M, template <typename> class Cont, typename K, typename V, typename Comp>
		friend class generic_flat_map_base;
		iterator_() = delete;
		iterator_(const typename It_Map::iterator& it) noexcept : iterator(it.iterator) {}
		iterator_& operator=(const typename It_Map::iterator& it) noexcept {
			iterator = it.iterator;
			return *this;
		}

		typename iterator_::reference operator*() const noexcept {
			return *iterator;
		}
		typename iterator_::pointer operator->() const noexcept {
			return iterator.operator->();
		}
		bool operator==(const iterator_& it) const noexcept {
			return it.iterator == iterator;
		}
		bool operator!=(const iterator_& it) const noexcept {
			return !(*this == it);
		}
		iterator_& operator++() noexcept {
			++iterator;
			return *this;
		}
		iterator_ operator++(int) noexcept {
			auto it = *this;
			this->operator++();
			return it;
		}
		iterator_& operator--() noexcept {
			--iterator;
			return *this;
		}
		iterator_ operator--(int) noexcept {
			auto it = *this;
			this->operator--();
			return it;
		}
		iterator_& operator+=(typename iterator_::difference_type n) noexcept {
			iterator += n;
			return *this;
		}
		iterator_& operator-=(typename iterator_::difference_type n) noexcept {
			iterator -= n;
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
			return it1.iterator - it2.iterator;
		}
		typename iterator_::reference operator[](int n) noexcept {
			return iterator[n];
		}
		bool operator<(iterator_ other) noexcept {
			return iterator < other.iterator;
		}
		bool operator>(iterator_ other) noexcept {
			return iterator > other.iterator;
		}
		bool operator<=(iterator_ other) noexcept {
			return iterator <= other.iterator;
		}
		bool operator>=(iterator_ other) noexcept {
			return iterator >= other.iterator;
		}
	};

	typedef iterator_<Map, typename container_t::iterator, Key, Value> iterator;
	typedef iterator_<Map, typename container_t::const_iterator, const Key, const Value> const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	void clear() noexcept {
		values.clear();
	}

	iterator erase(iterator pos) {
		return erase(const_iterator(pos));
	}
	iterator erase(const_iterator pos) {
		auto it = values.erase(pos.iterator);
		return iterator(it);
	}
	void swap(generic_flat_map_base& other) {
		using std::swap;
		swap(compare, other.compare);
		swap(values, other.values);
	}

	iterator find(const Key& key) {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		if(it != values.end())
			if(comp(key, it->first)) it = values.end();
		return iterator(it);
	}
	const_iterator find(const Key& key) const {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		if(it != values.end())
			if(comp(key, it->first)) it = values.end();
		return const_iterator(it);
	}
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
	iterator lower_bound(const Key& key) {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		return iterator(it);
	}
	const_iterator lower_bound(const Key& key) const {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		return const_iterator(it);
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator lower_bound(const K& key) {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		return iterator(it);
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator lower_bound(const K& key) const {
		key_compare comp(compare);
		auto it = std::lower_bound(values.begin(), values.end(), key, comp);
		return const_iterator(it);
	}
	iterator upper_bound(const Key& key) {
		key_compare comp(compare);
		auto it = std::upper_bound(values.begin(), values.end(), key, comp);
		return iterator(it);
	}
	const_iterator upper_bound(const Key& key) const {
		key_compare comp(compare);
		auto it = std::upper_bound(values.begin(), values.end(), key, comp);
		return const_iterator(it);
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	iterator upper_bound(const K& key) {
		key_compare comp(compare);
		auto it = std::upper_bound(values.begin(), values.end(), key, comp);
		return iterator(it);
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	const_iterator upper_bound(const K& key) const {
		key_compare comp(compare);
		auto it = std::upper_bound(values.begin(), values.end(), key, comp);
		return const_iterator(it);
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
		return values.empty();
	}

	size_t size() const noexcept {
		return values.size();
	}

	iterator begin() noexcept {
		return iterator(values.begin());
	}
	const_iterator begin() const noexcept {
		return const_iterator(values.begin());
	}
	const_iterator cbegin() const noexcept {
		return const_iterator(values.cbegin());
	}

	iterator end() noexcept {
		return iterator(values.end());
	}
	const_iterator end() const noexcept {
		return const_iterator(values.end());
	}
	const_iterator cend() const noexcept {
		return const_iterator(values.end());
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
	void resort() {
		std::stable_sort(values.begin(), values.end(),
						 [this](const auto& a, const auto& b) { return compare(a.first, b.first); });
	}
};

template <typename Map, template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(generic_flat_map_base<Map, Container, Key, Value, Compare>& m1,
		  generic_flat_map_base<Map, Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}
} // namespace detail

// Interface resembling that of the STL map containers
// But iterators return a std::pair<Key,Value> instead of std::pair<const Key, Value> because the latter one
// can't be portably combined with reallocating containers. (see
// http://stackoverflow.com/questions/14272141/is-casting-stdpairt1-t2-const-to-stdpairt1-const-t2-const-safe
// for explanation.) If the Key is modified by the user, the resort()
// member function has to be called to restore the invariants of the map.
template <template <typename> class Container, typename Key, typename Value, typename Compare = std::less<>>
class generic_flat_map
		: public detail::generic_flat_map_base<generic_flat_map<Container, Key, Value, Compare>, Container,
											   Key, Value, Compare> {

	typedef detail::generic_flat_map_base<generic_flat_map<Container, Key, Value, Compare>, Container, Key,
										  Value, Compare> Base;

public:
	template <typename... Args>
	generic_flat_map(Args&&... args) noexcept(std::is_nothrow_default_constructible<
			Compare>::value&& std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: generic_flat_map(Compare(), std::forward<Args>(args)...) {}
	template <typename... Args>
	explicit generic_flat_map(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(compare, std::forward<Args>(args)...) {}
	template <typename... Args>
	explicit generic_flat_map(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(std::move_if_noexcept(compare), std::forward<Args>(args)...) {}
	generic_flat_map(const generic_flat_map& other) noexcept(std::is_nothrow_copy_constructible<Base>::value)
			: Base(other) {}
	generic_flat_map(generic_flat_map&& other) noexcept(std::is_nothrow_move_constructible<Base>::value)
			: Base(std::move(other)) {}
	generic_flat_map&
	operator=(const generic_flat_map& other) noexcept(std::is_nothrow_copy_assignable<Base>::value) {
		Base::operator=(other);
		return *this;
	}
	generic_flat_map&
	operator=(generic_flat_map&& other) noexcept(std::is_nothrow_move_assignable<Base>::value) {
		Base::operator=(std::move(other));
		return *this;
	}

	typedef typename Base::key_compare key_compare;
	typedef typename Base::iterator iterator;
	typedef typename Base::const_iterator const_iterator;
	typedef typename Base::reverse_iterator reverse_iterator;
	typedef typename Base::const_reverse_iterator const_reverse_iterator;

	template <typename K, typename V>
	std::pair<iterator, bool> insert(K&& key, V&& value) {
		key_compare comp(this->compare);
		auto it_after = std::upper_bound(this->values.begin(), this->values.end(), key, comp);
		auto it = it_after;
		if(it != this->values.begin()) --it;
		if(!comp(*it, key) && !comp(key, *it)) return std::make_pair(iterator(it), false);
		this->values.reserve(this->values.size() + 1);
		it = this->values.emplace(it_after, std::forward<K>(key), std::forward<V>(value));
		return std::make_pair(iterator(it), true);
	}

	template <typename K, typename V>
	std::pair<iterator, bool> insert_or_assign(K&& key, V&& value) {
		key_compare comp(this->compare);
		auto it_after = std::upper_bound(this->values.begin(), this->values.end(), key, comp);
		auto it = it_after;
		if(it != this->values.begin()) --it;
		if(!comp(*it, key) && !comp(key, *it)) {
			it->second = std::forward<V>(value);
			return std::make_pair(iterator(it), false);
		}
		this->values.reserve(this->values.size() + 1);
		it = this->values.emplace(it_after, std::forward<K>(key), std::forward<V>(value));
		return std::make_pair(iterator(it), true);
	}

	using Base::erase;

	size_t erase(const Key& key) {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) return 0;
			erase(iterator(it));
			return 1;
		} else { return 0; }
	}
	size_t count(const Key& key) const {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) return 0;
			return 1;
		} else { return 0; }
	}
	template <typename K,
			  // Only allow this overload for transparent Compares:
			  typename Comp = Compare, typename X = typename Comp::is_transparent>
	size_t count(const K& key) const {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) return 0;
			return 1;
		} else { return 0; }
	}

	Value& at(const Key& key) {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) std::out_of_range("Key not found.");
			return it->second;
		} else { throw std::out_of_range("Key not found."); }
	}
	const Value& at(const Key& key) const {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			if(comp(key, *it)) std::out_of_range("Key not found.");
			return it->second;
		} else { throw std::out_of_range("Key not found."); }
	}
	template <typename K>
	Value& operator[](K&& key) {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end())
			if(comp(key, *it)) it = this->values.end();
		if(it == this->values.end()) { std::tie(it, std::ignore) = insert(std::forward<K>(key), Value()); }
		return *it;
	}

	void swap(generic_flat_map& other) {
		Base::swap(other);
	}
};

template <template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(generic_flat_map<Container, Key, Value, Compare>& m1,
		  generic_flat_map<Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

// Interface resembling that of the STL map containers
// But iterators return a std::pair<Key,Value> instead of std::pair<const Key, Value> because the latter one
// can't be portably combined with reallocating containers. (see
// http://stackoverflow.com/questions/14272141/is-casting-stdpairt1-t2-const-to-stdpairt1-const-t2-const-safe
// for explanation.) If the Key is modified by the user, the resort()
// member function has to be called to restore the invariants of the map.
template <template <typename> class Container, typename Key, typename Value, typename Compare = std::less<>>
class generic_flat_multimap
		: public detail::generic_flat_map_base<generic_flat_multimap<Container, Key, Value, Compare>,
											   Container, Key, Value, Compare> {

	typedef detail::generic_flat_map_base<generic_flat_multimap<Container, Key, Value, Compare>, Container,
										  Key, Value, Compare> Base;

public:
	template <typename... Args>
	generic_flat_multimap(Args&&... args) noexcept(std::is_nothrow_default_constructible<
			Compare>::value&& std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: generic_flat_multimap(Compare(), std::forward<Args>(args)...) {}
	template <typename... Args>
	explicit generic_flat_multimap(const Compare& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(compare, std::forward<Args>(args)...) {}
	template <typename... Args>
	explicit generic_flat_multimap(Compare&& compare, Args&&... args) noexcept(
			std::is_nothrow_constructible<Base, Compare, Args...>::value)
			: Base(std::move_if_noexcept(compare), std::forward<Args>(args)...) {}
	generic_flat_multimap(const generic_flat_multimap& other) noexcept(
			std::is_nothrow_copy_constructible<Base>::value)
			: Base(other) {}
	generic_flat_multimap(generic_flat_multimap&& other) noexcept(
			std::is_nothrow_move_constructible<Base>::value)
			: Base(std::move(other)) {}
	generic_flat_multimap&
	operator=(const generic_flat_multimap& other) noexcept(std::is_nothrow_copy_assignable<Base>::value) {
		Base::operator=(other);
		return *this;
	}
	generic_flat_multimap&
	operator=(generic_flat_multimap&& other) noexcept(std::is_nothrow_move_assignable<Base>::value) {
		Base::operator=(std::move(other));
		return *this;
	}

	typedef typename Base::key_compare key_compare;
	typedef typename Base::iterator iterator;
	typedef typename Base::const_iterator const_iterator;
	typedef typename Base::reverse_iterator reverse_iterator;
	typedef typename Base::const_reverse_iterator const_reverse_iterator;

	using Base::erase;
	template <typename K, typename V>
	iterator insert(K&& key, V&& value) {
		key_compare comp(this->compare);
		auto it_after = std::upper_bound(this->values.begin(), this->values.end(), key, comp);
		this->values.reserve(this->values.size() + 1);
		auto it = this->values.emplace(it_after, std::forward<K>(key), std::forward<V>(value));
		return iterator(it);
	}

	size_t erase(const Key& key) {
		key_compare comp(this->compare);
		auto it = std::lower_bound(this->values.begin(), this->values.end(), key, comp);
		if(it != this->values.end()) {
			size_t count = 0;
			auto erase_check = [this, &comp](auto&& it, auto&& key) {
				if(it == this->values.end()) return false;
				return !comp(key, *it);
			};
			while(erase_check(it, key)) {
				it = erase(iterator(it)).iterator;
				++count;
			}
			return count;
		} else { return 0; }
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
	void swap(generic_flat_multimap& other) {
		Base::swap(other);
	}
};

template <template <typename> class Container, typename Key, typename Value, typename Compare>
void swap(generic_flat_multimap<Container, Key, Value, Compare>& m1,
		  generic_flat_multimap<Container, Key, Value, Compare>& m2) {
	m1.swap(m2);
}

} // namespace containers
} // namespace mce

#endif /* CONTAINERS_GENERIC_FLAT_MAP_HPP_ */
