/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/local_function.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_LOCAL_FUNCTION_HPP_
#define UTIL_LOCAL_FUNCTION_HPP_

#include <cassert>
#include <functional>
#include <memory/align.hpp>
#include <memory>
#include <type_traits>

namespace mce {
namespace util {

template <size_t Max_Size, typename Signature>
class local_function;

namespace detail_local_function {

template <typename R, typename... Args>
class abstract_function_object;

template <typename R, typename... Args>
class deleter {
public:
	void operator()(detail_local_function::abstract_function_object<R, Args...>* ptr) {
		ptr->~abstract_function_object();
	}
};

template <typename R, typename... Args>
class abstract_function_object {
public:
	virtual ~abstract_function_object() = default;
	virtual R operator()(Args... args) const = 0;
	virtual R operator()(Args... args) = 0;
	virtual std::unique_ptr<abstract_function_object, deleter<R, Args...>> copy_to(void* location) const = 0;
	virtual std::unique_ptr<abstract_function_object, deleter<R, Args...>> move_to(void* location) = 0;
};

} // namespace detail_local_function

template <size_t Max_Size, typename R, typename... Args>
class local_function<Max_Size, R(Args...)> {
	typedef std::unique_ptr<detail_local_function::abstract_function_object<R, Args...>,
							detail_local_function::deleter<R, Args...>>
			abstract_func_obj_ptr;
	template <typename F>
	class function_object : public detail_local_function::abstract_function_object<R, Args...> {
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
		function_object(T&& f) : f(std::forward<T>(f)) {}
		virtual ~function_object() = default;
		virtual R operator()(Args... args) const override {
			return const_call_helper<F>::call(f, std::forward<Args>(args)...);
		}
		virtual R operator()(Args... args) override {
			return f(std::forward<Args>(args)...);
		}
		virtual abstract_func_obj_ptr copy_to(void* location) const override {
			static_assert(sizeof(function_object<F>) <= Max_Size, "Insufficient space for function object.");
			void* ptr = location;
			size_t space = Max_Size;
			if(!memory::align(alignof(function_object<F>), sizeof(function_object<F>), ptr, space))
				throw std::bad_alloc();
			return abstract_func_obj_ptr(new(ptr) function_object<F>(f));
		}
		virtual abstract_func_obj_ptr move_to(void* location) override {
			static_assert(sizeof(function_object<F>) <= Max_Size, "Insufficient space for function object.");
			void* ptr = location;
			size_t space = Max_Size;
			if(!memory::align(alignof(function_object<F>), sizeof(function_object<F>), ptr, space))
				throw std::bad_alloc();
			return abstract_func_obj_ptr(new(ptr) function_object<F>(std::move(f)));
		}
	};
	template <typename T>
	struct is_valid_function_value : std::is_same<R, decltype(std::declval<T>()(std::declval<Args>()...))> {};

	template <size_t Max_Size_2, typename T, typename... Args_2>
	struct is_valid_function_value<local_function<Max_Size_2, T(Args_2...)>> : std::false_type {};

	char storage[Max_Size];
	abstract_func_obj_ptr function_obj{nullptr};

public:
	template <size_t Max_Size_2, typename T>
	friend class local_function;
	local_function() {}
	template <typename F, typename Dummy = std::enable_if_t<is_valid_function_value<std::decay_t<F>>::value>>
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

	local_function(const local_function& other) {
		function_obj = other.function_obj->copy_to(storage);
	}
	local_function(local_function&& other) {
		function_obj = other.function_obj->move_to(storage);
	}
	template <size_t Max_Size_2>
	local_function(const local_function<Max_Size_2, R(Args...)>& other) {
		function_obj = other.function_obj->copy_to(storage);
	}
	template <size_t Max_Size_2>
	local_function(local_function<Max_Size_2, R(Args...)>&& other) {
		function_obj = other.function_obj->move_to(storage);
	}
	template <size_t Max_Size_2, typename Dummy = std::enable_if_t<Max_Size != Max_Size_2>>
	local_function& operator=(const local_function<Max_Size_2, R(Args...)>& other) {
		// No self-destruction check because the objects are of different type and aliasing them would be
		// undefined behavior anyway
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj = other.function_obj->copy_to(storage);
		return *this;
	}
	template <size_t Max_Size_2, typename Dummy = std::enable_if_t<Max_Size != Max_Size_2>>
	local_function& operator=(local_function<Max_Size_2, R(Args...)>&& other) {
		// No self-destruction check because the objects are of different type and aliasing them would be
		// undefined behavior anyway
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj = other.function_obj->move_to(storage);
		return *this;
	}
	local_function& operator=(const local_function& other) {
		if(&other == this) return *this;
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj = other.function_obj->copy_to(storage);
		return *this;
	}
	local_function& operator=(local_function&& other) {
		assert(&other != this);
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj = other.function_obj->move_to(storage);
		return *this;
	}
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
	R operator()(Args... args) const {
		const detail_local_function::abstract_function_object<R, Args...>* f = function_obj.get();
		if(!f) throw std::bad_function_call();
		return (*f)(std::forward<Args>(args)...);
	}
	R operator()(Args... args) {
		detail_local_function::abstract_function_object<R, Args...>* f = function_obj.get();
		if(!f) throw std::bad_function_call();
		return (*f)(std::forward<Args>(args)...);
	}
	operator bool() {
		return function_obj;
	}
};

} // namespace util
} // namespace mce

#endif /* UTIL_LOCAL_FUNCTION_HPP_ */
