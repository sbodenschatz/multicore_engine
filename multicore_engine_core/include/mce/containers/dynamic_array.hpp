/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/dynamic_array.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_CONTAINERS_DYNAMIC_ARRAY_HPP_
#define MCE_CONTAINERS_DYNAMIC_ARRAY_HPP_

#include <iterator>
#include <mce/memory/align.hpp>
#include <mce/util/traits.hpp>
#include <memory>
#include <type_traits>

namespace mce {
namespace containers {

struct index_constructor_parameter_tag {};

namespace detail {

template <typename T>
struct dynamic_array_index_switch_helper {
	static T&& pass(std::remove_reference_t<T>& t, size_t) {
		return std::forward<T>(t);
	}
	static T&& pass(std::remove_reference_t<T>&& t, size_t) {
		return std::forward<T>(t);
	}
};

template <>
struct dynamic_array_index_switch_helper<index_constructor_parameter_tag> {
	static size_t pass(index_constructor_parameter_tag, size_t index) {
		return index;
	}
};

} // namespace detail

template <typename T>
class dynamic_array {
	std::unique_ptr<char[]> raw_data_;
	T* data_;
	size_t size_;

	void allocate(size_t size) {
		auto space = size * sizeof(T) + alignof(T);
		raw_data_ = std::make_unique<char[]>(space);
		void* aptr = raw_data_.get();
		if(!memory::align(alignof(T), size * sizeof(T), aptr, space)) {
			throw std::bad_alloc();
		}
		data_ = reinterpret_cast<T*>(aptr);
	}
	void free() {
		for(size_t i = 0; i < size_; ++i) {
			data_[i].~T();
		}
	}

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

	dynamic_array() : size_{0}, data_{nullptr} {}
	dynamic_array(size_type size, const_reference value) : data_{nullptr}, size_{0} {
		allocate(size);
		for(; size_ < size; ++size_) {
			try {
				new(data_ + size_) T(value);
			} catch(...) {
				free();
				throw;
			}
		}
	}
	template <typename... F,
			  typename = std::enable_if_t<util::conjunction<util::is_callable<F, size_t>...>::value>>
	dynamic_array(size_type size, F... init_func) : data_{nullptr}, size_{0} {
		allocate(size);
		for(size_type i = 0; size_ < size; ++size_, ++i) {
			try {
				new(data_ + size_) T(init_func(i)...);
			} catch(...) {
				free();
				throw;
			}
		}
	}
	dynamic_array(std::initializer_list<value_type> values) : data_{nullptr}, size_{0} {
		allocate(values.size());
		for(const auto& val : values) {
			try {
				new(data_ + size_) T(val);
			} catch(...) {
				free();
				throw;
			}
			++size_;
		}
	}
	template <typename... Args,
			  typename = std::enable_if_t<!util::conjunction<util::is_callable<Args, size_t>...>::value>>
	dynamic_array(size_type size, Args&&... args) : data_{nullptr}, size_{0} {
		allocate(size);
		for(size_type i = 0; size_ < size; ++size_, ++i) {
			try {
				new(data_ + size_) T(detail::dynamic_array_index_switch_helper<Args>::pass(args, i)...);
			} catch(...) {
				free();
				throw;
			}
		}
	}
	dynamic_array(const dynamic_array& other) : data_{nullptr}, size_{0} {
		allocate(other.size());
		for(const auto& val : other) {
			try {
				new(data_ + size_) T(val);
			} catch(...) {
				free();
				throw;
			}
			++size_;
		}
	}
	dynamic_array& operator=(const dynamic_array& other) {
		if(this == std::addressof(other)) return *this;
		free();
		if(size_ != other.size_) {
			allocate(other.size_);
		}
		size_ = 0;
		for(const auto& val : other) {
			try {
				new(data_ + size_) T(val);
			} catch(...) {
				free();
				throw;
			}
			++size_;
		}
		return *this;
	}
	dynamic_array(dynamic_array&& other) noexcept
			: raw_data_{std::move(other.raw_data_)}, data_{other.data_}, size_{other.size_} {
		other.data_ = nullptr;
		other.size_ = 0;
	}
	dynamic_array& operator=(dynamic_array&& other) noexcept {
		free();
		raw_data_ = std::move(other.raw_data_);
		data_ = other.data_;
		size_ = other.size_;
		other.data_ = nullptr;
		other.size_ = 0;
		return *this;
	}
	~dynamic_array() noexcept {
		free();
	}
	reference at(size_type pos) {
		if(pos < size_)
			return data_[pos];
		else
			throw std::out_of_range("Position out of bounds.");
	}
	const_reference at(size_type pos) const {
		if(pos < size_)
			return data_[pos];
		else
			throw std::out_of_range("Position out of bounds.");
	}
	reference operator[](size_type pos) {
		return data_[pos];
	}
	const_reference operator[](size_type pos) const {
		return data_[pos];
	}
	reference front() {
		return *data_;
	}
	const_reference front() const {
		return *data_;
	}
	reference back() {
		return data_[size_ - 1];
	}
	const_reference back() const {
		return data_[size_ - 1];
	}
	T* data() noexcept {
		return data_;
	}
	const T* data() const noexcept {
		return data_;
	}
	iterator begin() noexcept {
		return data_;
	}
	const_iterator begin() const noexcept {
		return data_;
	}
	const_iterator cbegin() const noexcept {
		return data_;
	}
	iterator end() noexcept {
		return data_ + size_;
	}
	const_iterator end() const noexcept {
		return data_ + size_;
	}
	const_iterator cend() const noexcept {
		return data_ + size_;
	}
	reverse_iterator rbegin() noexcept {
		return data_ + size_;
	}
	const_reverse_iterator rbegin() const noexcept {
		return data_ + size_;
	}
	const_reverse_iterator crbegin() const noexcept {
		return data_ + size_;
	}
	reverse_iterator rend() noexcept {
		return data_;
	}
	const_reverse_iterator rend() const noexcept {
		return data_;
	}
	const_reverse_iterator crend() const {
		return data_;
	}
	size_type size() const noexcept {
		return size_;
	}
	void swap(dynamic_array& other) noexcept {
		using std::swap;
		swap(raw_data_, other.raw_data_);
		swap(data_, other.data_);
		swap(size_, other.size_);
	}
	friend void swap(dynamic_array& a, dynamic_array& b) noexcept {
		a.swap(b);
	}
};

} // namespace containers
} // namespace mce

#endif /* MCE_CONTAINERS_DYNAMIC_ARRAY_HPP_ */
