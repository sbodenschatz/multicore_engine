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

/// \brief Place holder tag for element constructor parameter lists that is replaced with the element index
/// casted to type <code>Integral</code>.
template <typename Integral>
struct index_param_tag {
	static_assert(std::is_integral<Integral>::value,
				  "index_param_tag must be parameterized on an integral type.");
};

/// \brief Place holder tag for element constructor parameter lists that is replaced with the result of
/// calling a <code>Functor</code> function object with the element index as the single parameter of type
/// size_t.
template <typename Functor>
struct generator_param_tag {
	Functor f; ///< The function object to call.
	static_assert(util::is_callable<Functor, size_t>::value,
				  "generator_param_tag must be parameterized on a function object type callable with "
				  "a single size_t parameter.");
};

/// Template argument deduction helper function for generator_param_tag.
template <typename F>
generator_param_tag<std::remove_reference_t<F>> generator_param(F&& f) {
	return {std::forward<F>(f)};
}

namespace detail {

template <typename T>
struct dynamic_array_ctor_param_switch_helper {
	static T&& pass(std::remove_reference_t<T>& t, size_t) {
		return std::forward<T>(t);
	}
	static T&& pass(std::remove_reference_t<T>&& t, size_t) {
		return std::forward<T>(t);
	}
};

template <typename Integral>
struct dynamic_array_ctor_param_switch_helper<index_param_tag<Integral>> {
	static Integral pass(index_param_tag<Integral>, size_t index) {
		return Integral(index);
	}
};

template <typename Functor>
struct dynamic_array_ctor_param_switch_helper<generator_param_tag<Functor>> {
	static decltype(auto) pass(generator_param_tag<Functor> gt, size_t index) {
		return gt.f(index);
	}
};

template <typename T, bool>
struct construct_helper {
	template <typename... Args>
	static void construct(void* ptr, Args&&... args) {
		new(ptr) T(std::forward<Args>(args)...);
	}
};

template <typename T>
struct construct_helper<T, false> {
	template <typename... Args>
	static void construct(void* ptr, Args&&... args) {
		new(ptr) T{std::forward<Args>(args)...};
	}
};

} // namespace detail

/// Provides an array of type T with a runtime-determined size.
/**
 * In contrast to std::vector dynamic_array doen't support resizing and can thus avoid to impose the
 * requirements needed for reallocation on T.
 * The complete array can be move and copy assigned and constructed but the object doesn't support resizing
 * without replacing.
 */
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
	template <typename... Args, typename U = T>
	void construct(void* ptr, Args&&... args) {
		detail::construct_helper<U, std::is_constructible<U, Args...>::value>::construct(
				ptr, std::forward<Args>(args)...);
	}

public:
	/// The type of the values in the container.
	using value_type = T;
	/// The type used for sizes and indices.
	using size_type = std::size_t;
	/// The type used for references to the contents.
	using reference = value_type&;
	/// The type used for read-only references to the contents.
	using const_reference = const value_type&;
	/// The type used for pointers to the contents.
	using pointer = value_type*;
	/// The type used for read-only pointers to the contents.
	using const_pointer = const value_type*;
	/// The type of read-write iterators over the contents.
	using iterator = pointer;
	/// The type of read-only iterators over the contents.
	using const_iterator = const_pointer;
	/// The type of read-write iterators over the contents with reversed traversal.
	using reverse_iterator = std::reverse_iterator<iterator>;
	/// The type of read-only iterators over the contents with reversed traversal.
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	/// Creates an empty dynamic_array.
	dynamic_array() : data_{nullptr}, size_{0} {}

	/// Creates a dynamic_array containing the given number of copies of the given value.
	dynamic_array(size_type size, const_reference value) : data_{nullptr}, size_{0} {
		allocate(size);
		for(; size_ < size; ++size_) {
			try {
				construct(data_ + size_, value);
			} catch(...) {
				free();
				throw;
			}
		}
	}

	/// Creates a dynamic_array containing the values from the given std::initializer_list.
	explicit dynamic_array(std::initializer_list<value_type> values) : data_{nullptr}, size_{0} {
		allocate(values.size());
		for(const auto& val : values) {
			try {
				construct(data_ + size_, val);
			} catch(...) {
				free();
				throw;
			}
			++size_;
		}
	}

	/// \brief Creates a dynamic_array with the given number of values constructed by forwarding the given
	/// constructor arguments.
	/**
	 * Additionally to simply passing values, any of the parameters can be given as one of the following place
	 * holder tags:
	 * - index_param_tag<Integral> to pass the element index casted to Integral in it's place.
	 * - generator_param_tag<Functor> to pass in it's place the return value of calling operator()(size_t) of
	 * Functor with the element index. Such a generator_param tag can be created with template argument
	 * deduction using generator_param(F).
	 */
	template <typename... Args>
	dynamic_array(size_type size, Args&&... args) : data_{nullptr}, size_{0} {
		allocate(size);
		for(size_type i = 0; size_ < size; ++size_, ++i) {
			try {
				construct(data_ + size_,
						  detail::dynamic_array_ctor_param_switch_helper<Args>::pass(args, i)...);
			} catch(...) {
				free();
				throw;
			}
		}
	}

	/// Constructs a dynamic_array by copying the given dynamic_array and it's contents.
	dynamic_array(const dynamic_array& other) : data_{nullptr}, size_{0} {
		allocate(other.size());
		for(const auto& val : other) {
			try {
				construct(data_ + size_, val);
			} catch(...) {
				free();
				throw;
			}
			++size_;
		}
	}

	/// Replaces this dynamic_array with a copy of the given array.
	dynamic_array& operator=(const dynamic_array& other) {
		if(this == std::addressof(other)) return *this;
		free();
		if(size_ != other.size_) {
			allocate(other.size_);
		}
		size_ = 0;
		for(const auto& val : other) {
			try {
				construct(data_ + size_, val);
			} catch(...) {
				free();
				throw;
			}
			++size_;
		}
		return *this;
	}

	/// Allows move-construction.
	dynamic_array(dynamic_array&& other) noexcept
			: raw_data_{std::move(other.raw_data_)}, data_{other.data_}, size_{other.size_} {
		other.data_ = nullptr;
		other.size_ = 0;
	}

	/// Replaces this dynamic_array by moving the given array into it.
	dynamic_array& operator=(dynamic_array&& other) noexcept {
		free();
		raw_data_ = std::move(other.raw_data_);
		data_ = other.data_;
		size_ = other.size_;
		other.data_ = nullptr;
		other.size_ = 0;
		return *this;
	}

	/// Destroys the array and it's contents.
	~dynamic_array() noexcept {
		free();
	}

	/// Allows read-write access to the element with the given index with bounds check.
	reference at(size_type pos) {
		if(pos < size_)
			return data_[pos];
		else
			throw std::out_of_range("Position out of bounds.");
	}

	/// Allows read-only access to the element with the given index with bounds check.
	const_reference at(size_type pos) const {
		if(pos < size_)
			return data_[pos];
		else
			throw std::out_of_range("Position out of bounds.");
	}

	/// Allows read-write access to the element with the given index without bounds check.
	reference operator[](size_type pos) {
		return data_[pos];
	}

	/// Allows read-only access to the element with the given index without bounds check.
	const_reference operator[](size_type pos) const {
		return data_[pos];
	}

	/// Allows access to the first element of the dynamic_array.
	/**
	 * Calling this on an empty container invokes undefined behavior.
	 */
	reference front() {
		return *data_;
	}

	/// Allows read-only access to the first element of the dynamic_array.
	/**
	 * Calling this on an empty container invokes undefined behavior.
	 */
	const_reference front() const {
		return *data_;
	}

	/// Allows access to the last element of the dynamic_array.
	/**
	 * Calling this on an empty container invokes undefined behavior.
	 */
	reference back() {
		return data_[size_ - 1];
	}

	/// Allows read-only access to the last element of the dynamic_array.
	/**
	 * Calling this on an empty container invokes undefined behavior.
	 */
	const_reference back() const {
		return data_[size_ - 1];
	}

	/// Returns a pointer to the contents of the container.
	pointer data() noexcept {
		return data_;
	}

	/// Returns a read-only pointer to the contents of the container.
	const_pointer data() const noexcept {
		return data_;
	}

	/// Returns an read-write iterator referring to the beginning of the container.
	iterator begin() noexcept {
		return data_;
	}

	/// Returns an read-only iterator referring to the beginning of the container.
	const_iterator begin() const noexcept {
		return data_;
	}

	/// Returns an read-only iterator referring to the beginning of the container.
	const_iterator cbegin() const noexcept {
		return data_;
	}

	/// Returns an read-write iterator referring to the end of the container.
	iterator end() noexcept {
		return data_ + size_;
	}

	/// Returns an read-only iterator referring to the end of the container.
	const_iterator end() const noexcept {
		return data_ + size_;
	}

	/// Returns an read-only iterator referring to the end of the container.
	const_iterator cend() const noexcept {
		return data_ + size_;
	}

	/// Returns an read-write iterator referring to the beginning of the container in reverse order.
	reverse_iterator rbegin() noexcept {
		return data_ + size_;
	}

	/// Returns an read-only iterator referring to the beginning of the container in reverse order.
	const_reverse_iterator rbegin() const noexcept {
		return data_ + size_;
	}

	/// Returns an read-only iterator referring to the beginning of the container in reverse order.
	const_reverse_iterator crbegin() const noexcept {
		return data_ + size_;
	}

	/// Returns an read-write iterator referring to the end of the container in reverse order.
	reverse_iterator rend() noexcept {
		return data_;
	}

	/// Returns an read-only iterator referring to the end of the container in reverse order.
	const_reverse_iterator rend() const noexcept {
		return data_;
	}

	/// Returns an read-only iterator referring to the end of the container in reverse order.
	const_reverse_iterator crend() const {
		return data_;
	}

	/// Returns the number of elements in the dynamic_array.
	size_type size() const noexcept {
		return size_;
	}

	/// Swaps the contents of *this and other.
	void swap(dynamic_array& other) noexcept {
		using std::swap;
		swap(raw_data_, other.raw_data_);
		swap(data_, other.data_);
		swap(size_, other.size_);
	}

	/// Swaps the contents of both given dynamic_array objects.
	friend void swap(dynamic_array& a, dynamic_array& b) noexcept {
		a.swap(b);
	}
};

} // namespace containers
} // namespace mce

#endif /* MCE_CONTAINERS_DYNAMIC_ARRAY_HPP_ */
