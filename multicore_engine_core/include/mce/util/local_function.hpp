/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/local_function.hpp
 * Copyright 2015-2017 by Stefan Bodenschatz
 */

#ifndef UTIL_LOCAL_FUNCTION_HPP_
#define UTIL_LOCAL_FUNCTION_HPP_

/**
 * \file
 * Defines a generic function wrapper class with type erasure without heap allocations.
 */

#include <cassert>
#include <cstdint>
#include <functional>
#include <mce/memory/align.hpp>
#include <memory>
#include <new>
#include <type_traits>

namespace mce {
namespace util {

/// See \link mce::util::local_function< Max_Size, R(Args...)> specialization for function signatures\endlink.
template <size_t Max_Size, typename Signature>
class local_function;

namespace detail {
namespace local_function {

template <typename R, typename... Args>
class abstract_function_object;

template <typename R, typename... Args>
class deleter {
public:
	void operator()(detail::local_function::abstract_function_object<R, Args...>* ptr) {
		ptr->~abstract_function_object();
	}
};

template <typename R, typename... Args>
class abstract_function_object {
public:
	virtual ~abstract_function_object() = default;
	virtual R operator()(Args... args) const = 0;
	virtual R operator()(Args... args) = 0;
	virtual std::unique_ptr<abstract_function_object, deleter<R, Args...>>
	copy_to(void* location, size_t storage_size) const = 0;
	virtual std::unique_ptr<abstract_function_object, deleter<R, Args...>> move_to(void* location,
																				   size_t storage_size) = 0;
};

} // namespace local_function
} // namespace detail

/// Provides a generic type-erasing function wrapper, that keeps the functor inside the wrapper object.
/**
 * This class has a similar use-case as std::function, like std::function it is a generic function wrapper
 * providing type erasure of the concrete function type. What mainly differentiates this class from the
 * standard counterpart, is that the std::function keeps the functor object on the heap in a separate
 * allocation if it exceeds the narrow size limitation of the small functor optimization used by the STL
 * implementation. This class always stores the functors in a buffer inside the object itself and therefore
 * prevents a separate heap allocation. This allows more efficient construction and assignment of function
 * objects. It also allows better memory contiguousness as the functors are not scattered across the address
 * space. However this comes at some costs:
 * 1. The buffer size must be specified by the using code and if functor size exceeds it an exception is
 * thrown when allocating the functor object.
 * 2. When local_function objects are moved, the stored functor can not be handed over by pointer but must
 * itself be moved to a new memory location.
 * 3. The local_function object will likely be significantly larger than an equivalent std::function object
 * depending on the given buffer size. This will especially be the case if the using code has no definitive
 * knowledge of the functor size and has to assume a big buffer size. However cases with pre-known functor
 * sizes might be equally memory efficient as std::function or even a bit more efficient as it considering
 * memory manger overhead.
 * 4. As there is only one buffer for functors to live in, the local_function can only provide the basic
 * exception guarantee.
 */
template <size_t Max_Size, typename R, typename... Args>
class local_function<Max_Size, R(Args...)> {
	typedef std::unique_ptr<detail::local_function::abstract_function_object<R, Args...>,
							detail::local_function::deleter<R, Args...>>
			abstract_func_obj_ptr;
	template <typename F>
	class function_object : public detail::local_function::abstract_function_object<R, Args...> {
		F f;

		template <typename = void, typename = void>
		struct const_call_helper {
			static R call(const F&, Args...) {
				throw std::bad_function_call();
			}
		};

		template <typename T>
		struct const_call_helper<
				T, std::enable_if_t<std::is_same<R, decltype(std::declval<const T>()(
															std::declval<Args>()...))>::value>> {
			static R call(const F& f, Args... args) {
				return f(std::forward<Args>(args)...);
			}
		};

	public:
		template <typename T>
		// cppcheck-suppress noExplicitConstructor
		function_object(T&& f) : f(std::forward<T>(f)) {}
		virtual ~function_object() = default;
		virtual R operator()(Args... args) const override {
			return const_call_helper<F>::call(f, std::forward<Args>(args)...);
		}
		virtual R operator()(Args... args) override {
			return f(std::forward<Args>(args)...);
		}
		virtual abstract_func_obj_ptr copy_to(void* location, size_t storage_size) const override {
			void* ptr = location;
			size_t space = storage_size;
			if(!memory::align(alignof(function_object<F>), sizeof(function_object<F>), ptr, space))
				throw std::bad_alloc();
			return abstract_func_obj_ptr(new(ptr) function_object<F>(f));
		}
		virtual abstract_func_obj_ptr move_to(void* location, size_t storage_size) override {
			void* ptr = location;
			size_t space = storage_size;
			if(!memory::align(alignof(function_object<F>), sizeof(function_object<F>), ptr, space))
				throw std::bad_alloc();
			return abstract_func_obj_ptr(new(ptr) function_object<F>(std::move(f)));
		}
	};
	template <typename T>
	struct is_valid_function_value : std::is_same<R, decltype(std::declval<T>()(std::declval<Args>()...))> {};

	template <size_t Max_Size_2, typename T, typename... Args_2>
	struct is_valid_function_value<local_function<Max_Size_2, T(Args_2...)>> : std::false_type {};

	char storage[Max_Size]{};
	abstract_func_obj_ptr function_obj{nullptr};

public:
	template <size_t Max_Size_2, typename T>
	friend class local_function;
	/// Creates an empty local_function object.
	/**
	 * Calling such an empty object results in an exception of type std::bad_function_call being thrown.
	 */
	local_function() {}
	/// Constructs a function_object from the given forwarding reference to a function object.
	/**
	 * The function object is either copied or moved into the local_function object depending on the value
	 * category of the parameter. The type F after applying decay must be callable with the signature of the
	 * local_function (R(Args...)).
	 *
	 * If the function object is statically too big for the buffer size of the local_function, a static_assert
	 * is triggered. If it doesn't fit in the buffer dynamically (due to alignment requirements of the functor
	 * type and padding space) a exception of type std::bad_alloc is thrown.
	 */
	template <typename F, typename Dummy = std::enable_if_t<is_valid_function_value<std::decay_t<F>>::value>>
	// cppcheck-suppress noExplicitConstructor
	local_function(F&& f) {
		static_assert(sizeof(function_object<std::decay_t<F>>) <= Max_Size,
					  "Insufficient space for function object.");
		void* ptr = storage;
		size_t space = Max_Size;
		if(!memory::align(alignof(function_object<std::decay_t<F>>), sizeof(function_object<std::decay_t<F>>),
						  ptr, space))
			throw std::bad_alloc();
		function_obj.reset(new(ptr) function_object<std::decay_t<F>>(std::forward<F>(f)));
	}

	/// Allows copying.
	local_function(const local_function& other) {
		function_obj = other.function_obj->copy_to(storage, Max_Size);
	}
	/// Allows moving.
	local_function(local_function&& other) {
		function_obj = other.function_obj->move_to(storage, Max_Size);
	}
	/// Allows copying of smaller function wrappers.
	template <size_t Max_Size_2, typename Dummy = std::enable_if_t<Max_Size >= Max_Size_2>>
	local_function(const local_function<Max_Size_2, R(Args...)>& other) {
		function_obj = other.function_obj->copy_to(storage, Max_Size);
	}
	/// Allows moving of smaller function wrappers.
	template <size_t Max_Size_2, typename Dummy = std::enable_if_t<Max_Size >= Max_Size_2>>
	local_function(local_function<Max_Size_2, R(Args...)>&& other) {
		function_obj = other.function_obj->move_to(storage, Max_Size);
	}
	/// Allows copying of smaller function wrappers.
	template <size_t Max_Size_2, typename Dummy = std::enable_if_t<Max_Size >= Max_Size_2>>
	local_function& operator=(const local_function<Max_Size_2, R(Args...)>& other) {
		// No self-destruction check because the objects are of different type and aliasing them would be
		// undefined behavior anyway
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj = other.function_obj->copy_to(storage, Max_Size);
		return *this;
	}
	/// Allows moving of smaller function wrappers.
	template <size_t Max_Size_2, typename Dummy = std::enable_if_t<Max_Size >= Max_Size_2>>
	local_function& operator=(local_function<Max_Size_2, R(Args...)>&& other) {
		// No self-destruction check because the objects are of different type and aliasing them would be
		// undefined behavior anyway
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj = other.function_obj->move_to(storage, Max_Size);
		return *this;
	}
	/// Allows copying.
	local_function& operator=(const local_function& other) {
		if(&other == this) return *this;
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj = other.function_obj->copy_to(storage, Max_Size);
		return *this;
	}
	/// Allows moving.
	local_function& operator=(local_function&& other) {
		assert(&other != this);
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj = other.function_obj->move_to(storage, Max_Size);
		return *this;
	}
	/// Assigns a new functor to the function wrapper object (see local_function(F&&) for requirements).
	template <typename F, typename Dummy = std::enable_if_t<is_valid_function_value<std::decay_t<F>>::value>>
	local_function& operator=(F&& new_f) {
		static_assert(sizeof(function_object<std::decay_t<F>>) <= Max_Size,
					  "Insufficient space for function object.");
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		void* ptr = storage;
		size_t space = Max_Size;
		if(!memory::align(alignof(function_object<std::decay_t<F>>), sizeof(function_object<std::decay_t<F>>),
						  ptr, space))
			throw std::bad_alloc();
		function_obj.reset(new(ptr) function_object<std::decay_t<F>>(std::forward<F>(new_f)));
		return *this;
	}
	/// Calls the wrapped function object with the functor being const.
	R operator()(Args... args) const {
		const detail::local_function::abstract_function_object<R, Args...>* f = function_obj.get();
		if(!f) throw std::bad_function_call();
		return (*f)(std::forward<Args>(args)...);
	}
	/// Calls the wrapped function object with the functor being non-const.
	R operator()(Args... args) {
		detail::local_function::abstract_function_object<R, Args...>* f = function_obj.get();
		if(!f) throw std::bad_function_call();
		return (*f)(std::forward<Args>(args)...);
	}
	/// Returns true if the wrapper contains a functor and false if it is empty.
	operator bool() {
		return function_obj;
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_LOCAL_FUNCTION_HPP_ */
