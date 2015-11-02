/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/util/local_function.hpp
 * Copyright 2015 by Stefan Bodenschatz
 */

#ifndef UTIL_LOCAL_FUNCTION_HPP_
#define UTIL_LOCAL_FUNCTION_HPP_

#include <type_traits>
#include <memory>
#include <cassert>
#include <memory/align.hpp>
#include <functional>

namespace mce {
namespace util {

template <size_t, typename>
class local_function;

template <size_t Max_Size, typename R, typename... Args>
class local_function<Max_Size, R(Args...)> {
	class abstract_function_object {
	public:
		virtual ~abstract_function_object() = default;
		virtual R operator()(Args... args) const = 0;
		virtual R operator()(Args... args) = 0;
		virtual abstract_function_object* copy_to(void* location) const = 0;
		virtual abstract_function_object* move_to(void* location) = 0;
	};
	template <typename F>
	class function_object : public abstract_function_object {
		F f;

	public:
		template <typename T>
		function_object(T&& f)
				: f(std::forward<T>(f)) {}
		virtual R operator()(Args... args) const override {
			return f(std::forward<Args>(args)...);
		}
		virtual R operator()(Args... args) override {
			return f(std::forward<Args>(args)...);
		}
		virtual abstract_function_object* copy_to(void* location) const override {
			void* ptr = location;
			size_t space = Max_Size;
			if(!memory::align(alignof(function_object<F>), sizeof(function_object<F>), ptr, space))
				throw std::bad_alloc();
			return new(ptr) function_object<F>(f);
		}
		virtual abstract_function_object* move_to(void* location) override {
			void* ptr = location;
			size_t space = Max_Size;
			if(!memory::align(alignof(function_object<F>), sizeof(function_object<F>), ptr, space))
				throw std::bad_alloc();
			return new(ptr) function_object<F>(std::move(f));
		}
	};
	class deleter {
	public:
		void operator()(abstract_function_object* ptr) {
			ptr->~abstract_function_object();
		}
	};

	char storage[Max_Size];
	std::unique_ptr<abstract_function_object, deleter> function_obj{nullptr};

public:
	template <size_t Max_Size_2, typename T>
	friend class local_function;
	local_function() {}
	// TODO: Restrict participation in overload resolution
	template <typename F>
	local_function(F f) {
		void* ptr = storage;
		size_t space = Max_Size;
		if(!memory::align(alignof(function_object<F>), sizeof(function_object<F>), ptr, space))
			throw std::bad_alloc();
		function_obj.reset(new(ptr) function_object<F>(std::move(f)));
	}
	template <size_t Max_Size_2>
	local_function(const local_function<Max_Size_2, R(Args...)>& other) {
		function_obj.reset(other.function_obj->copy_to(storage));
	}
	template <size_t Max_Size_2>
	local_function(local_function<Max_Size_2, R(Args...)>&& other) {
		function_obj.reset(other.function_obj->move_to(storage));
	}
	template <size_t Max_Size_2>
	local_function& operator=(const local_function<Max_Size_2, R(Args...)>& other) {
		if(&other == this) return *this;
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj.reset(other.function_obj->copy_to(storage));
		return *this;
	}
	template <size_t Max_Size_2>
	local_function& operator=(local_function<Max_Size_2, R(Args...)>&& other) {
		assert(&other != this);
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		function_obj.reset(other.function_obj->move_to(storage));
	}
	template <typename F>
	local_function& operator=(F&& new_f) {
		function_obj.reset(); // Destroy current value first because storage is needed for new value
		void* ptr = storage;
		size_t space = Max_Size;
		if(!memory::align(alignof(function_object<F>), sizeof(function_object<F>), ptr, space))
			throw std::bad_alloc();
		function_obj.reset(new(ptr) function_object<F>(std::forward<F>(new_f)));
	}
	R operator()(Args... args) const {
		const abstract_function_object* f = function_obj.get();
		if(!f) throw std::bad_function_call();
		return (*f)(std::forward<Args>(args)...);
	}
	R operator()(Args... args) {
		abstract_function_object* f = function_obj.get();
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
