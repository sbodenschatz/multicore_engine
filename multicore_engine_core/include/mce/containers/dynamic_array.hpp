/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/dynamic_array.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_CONTAINERS_DYNAMIC_ARRAY_HPP_
#define MCE_CONTAINERS_DYNAMIC_ARRAY_HPP_

#include <iterator>
#include <memory>
#include <type_traits>

namespace mce {
namespace containers {

template <typename T>
class dynamic_array {
	std::unique_ptr<char[]> data_;
	size_t size_;

public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	dynamic_array();
	dynamic_array(size_type size, const_reference value);
	template <typename F>
	dynamic_array(size_type size, F initialization_function);
	dynamic_array(std::initializer_list<value_type> values);
	dynamic_array(std::initializer_list<std::reference_wrapper<const value_type>> values);
	dynamic_array(const dynamic_array& other);
	dynamic_array& operator=(const dynamic_array& other);
	dynamic_array(dynamic_array&&) noexcept;
	dynamic_array& operator=(dynamic_array&&) noexcept;
	reference at(size_type pos);
	const_reference at(size_type pos) const;
	reference operator[](size_type pos);
	const_reference operator[](size_type pos) const;
	reference front();
	const_reference front() const;
	reference back();
	const_reference back() const;
	T* data() noexcept;
	const T* data() const noexcept;
	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;
	reverse_iterator rbegin() noexcept;
	const_reverse_iterator rbegin() const noexcept;
	const_reverse_iterator crbegin() const noexcept;
	reverse_iterator rend() noexcept;
	const_reverse_iterator rend() const noexcept;
	const_reverse_iterator crend() const;
	size_type size() const noexcept;
	void swap(dynamic_array& other) noexcept;
	friend void swap(dynamic_array& a, dynamic_array& b) noexcept {}
};

} // namespace containers
} // namespace mce

#endif /* MCE_CONTAINERS_DYNAMIC_ARRAY_HPP_ */
