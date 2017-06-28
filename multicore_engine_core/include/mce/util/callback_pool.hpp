/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/util/callback_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_UTIL_CALLBACK_POOL_HPP_
#define MCE_UTIL_CALLBACK_POOL_HPP_

#include <algorithm>
#include <atomic>
#include <cassert>
#include <mce/memory/align.hpp>
#include <memory>
#include <mutex>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

namespace mce {
namespace util {

namespace detail {

template <typename>
class callback_pool_function_iface {};

template <typename R, typename... Args>
class callback_pool_function_iface<R(Args...)> {
public:
	virtual ~callback_pool_function_iface() = default;
	virtual R operator()(Args...) = 0;
	virtual R operator()(Args...) const = 0;
};

template <typename, typename>
class callback_pool_function_impl {};

template <typename F, typename R, typename... Args>
class callback_pool_function_impl<F, R(Args...)> final : public callback_pool_function_iface<R(Args...)> {
	F f;

	template <typename = void, typename = void>
	struct const_call_helper {
		static R call(const F&, Args...) {
			throw std::bad_function_call();
		}
	};

	template <typename T>
	struct const_call_helper<T, std::enable_if_t<std::is_same<R, decltype(std::declval<const T>()(
																		 std::declval<Args>()...))>::value>> {
		static R call(const F& f, Args... args) {
			return f(std::forward<Args>(args)...);
		}
	};

public:
	template <typename T>
	callback_pool_function_impl(T&& fun)
			: f{std::forward<T>(fun)} {}
	virtual ~callback_pool_function_impl() = default;
	virtual R operator()(Args... args) override {
		return f(std::forward<Args>(args)...);
	}
	virtual R operator()(Args... args) const override {
		return const_call_helper<F>::call(f, std::forward<Args>(args)...);
	}
};

class callback_pool_buffer {
	char* data_;
	size_t size_;
	std::atomic<size_t> ref_count_{0};

public:
	callback_pool_buffer(char* data, size_t size) noexcept : data_{data}, size_{size} {}

	size_t increment_ref_count() noexcept {
		return ++ref_count_;
	}

	size_t decrement_ref_count() noexcept {
		return --ref_count_;
	}

	size_t ref_count() const noexcept {
		return ref_count_.load();
	}

	size_t size() const noexcept {
		return size_;
	}

	char* data() const noexcept {
		return data_;
	}
};

template <typename R, typename... Args>
class callback_pool_function_deleter {
	std::shared_ptr<detail::callback_pool_buffer> buffer;

public:
	callback_pool_function_deleter() noexcept {}
	// cppcheck-suppress noExplicitConstructor
	callback_pool_function_deleter(const std::shared_ptr<detail::callback_pool_buffer>& buffer) noexcept
			: buffer{buffer} {}
	// cppcheck-suppress noExplicitConstructor
	callback_pool_function_deleter(std::shared_ptr<detail::callback_pool_buffer>&& buffer) noexcept
			: buffer{std::move(buffer)} {}

	void operator()(callback_pool_function_iface<R(Args...)>* ptr) noexcept {
		ptr->~callback_pool_function_iface();
		assert(buffer);
		buffer->decrement_ref_count();
	}
};

class callback_pool_buffer_deleter {
	char* raw_buffer;

public:
	explicit callback_pool_buffer_deleter(char* raw_buffer) noexcept : raw_buffer{raw_buffer} {}

	void operator()(callback_pool_buffer* b) noexcept {
		b->~callback_pool_buffer();
		delete[] raw_buffer;
	}
};

} // namespace detail
template <typename>
class callback_pool_function {};

/// \brief Provides a type-erasing function wrapper similar to std::function that can take all function
/// objects with the given return value and argument types.
template <typename R, typename... Args>
class callback_pool_function<R(Args...)> {
	using ifo_t = std::unique_ptr<detail::callback_pool_function_iface<R(Args...)>,
								  detail::callback_pool_function_deleter<R, Args...>>;
	ifo_t internal_function_object;

	friend class callback_pool;

	template <typename T>
	struct is_valid_function_value : std::is_same<R, decltype(std::declval<T>()(std::declval<Args>()...))> {};

	template <typename R2, typename... Args_2>
	struct is_valid_function_value<callback_pool_function<R2(Args_2...)>> : std::false_type {};

	callback_pool_function(detail::callback_pool_function_iface<R(Args...)>* ptr,
						   const std::shared_ptr<detail::callback_pool_buffer>& buffer)
			: internal_function_object{std::move(ptr), buffer} {}

public:
	/// Creates an empty function wrapper, that throws an exception when called.
	callback_pool_function() noexcept;
	/// Allows move-construction.
	callback_pool_function(callback_pool_function&& other) noexcept
			: internal_function_object{std::move(other.internal_function_object)} {
		other.internal_function_object = nullptr;
	}
	/// Allows move-assignment.
	callback_pool_function& operator=(callback_pool_function&& other) noexcept {
		using std::swap;
		swap(internal_function_object, other.internal_function_object);
		other.internal_function_object.reset();
		return *this;
	}
	/// Calls the wrapped function with the function object being const.
	R operator()(Args... args) const {
		const detail::callback_pool_function_iface<R(Args...)>* f = internal_function_object.get();
		if(!f) throw std::bad_function_call();
		return (*f)(std::forward<Args>(args)...);
	}
	/// Calls the wrapped function with the function object being non-const.
	R operator()(Args... args) {
		detail::callback_pool_function_iface<R(Args...)>* f = internal_function_object.get();
		if(!f) throw std::bad_function_call();
		return (*f)(std::forward<Args>(args)...);
	}
	/// Checks if the function wrapper is non-empty.
	operator bool() noexcept {
		return internal_function_object;
	}
};

/// Provides a thread-safe memory pool for backing callback_pool_function objects.
class callback_pool {
	std::shared_ptr<detail::callback_pool_buffer> current_buffer;
	std::vector<std::shared_ptr<detail::callback_pool_buffer>> stashed_buffers;
	size_t current_buffer_offset = 0;
	mutable std::mutex pool_mutex;
	static constexpr size_t min_slots = 10;
	static constexpr size_t growth_factor = 2;
	static constexpr size_t min_buffer_size = size_t(1) << 20;

	void reallocate(size_t alloc_size);
	void* try_alloc_obj_block(size_t size, size_t alignment) const noexcept;
	size_t curr_buf_cap() const noexcept {
		return current_buffer ? current_buffer->size() : size_t(0);
	}

public:
	/// Creates a callback_pool.
	callback_pool() {}

	/// Allows move-construction.
	callback_pool(callback_pool&& other) noexcept;
	/// Allows move-assignment.
	callback_pool& operator=(callback_pool&& other) noexcept;

	/// Wraps the given function object into a callback_pool_function of the given signature.
	template <typename Signature, typename F,
			  typename = std::enable_if_t<callback_pool_function<Signature>::template is_valid_function_value<
					  std::decay_t<F>>::value>>
	callback_pool_function<Signature> allocate_function(F&& f) {
		std::lock_guard<std::mutex> lock(pool_mutex);
		using fun = detail::callback_pool_function_impl<F, Signature>;
		auto loc = try_alloc_obj_block(sizeof(fun), alignof(fun));
		if(!loc) {
			auto requested_size = sizeof(fun) > curr_buf_cap() ? sizeof(fun) * min_slots : curr_buf_cap();
			if(requested_size < min_buffer_size) requested_size = min_buffer_size;
			reallocate(requested_size);
			loc = try_alloc_obj_block(sizeof(fun), alignof(fun));
			assert(loc);
		}
		current_buffer->increment_ref_count();
		auto fun_ptr = new(loc) fun(std::forward<F>(f));
		current_buffer_offset = reinterpret_cast<char*>(fun_ptr + 1) - current_buffer->data();
		return callback_pool_function<Signature>(fun_ptr, current_buffer);
	}

	/// Ensures that the pool has at least the given size in buffer capacity (not necessarily free).
	void reserve(size_t size) {
		std::lock_guard<std::mutex> lock(pool_mutex);
		if(!current_buffer || current_buffer->size() < size) {
			reallocate(size);
		}
	}

	/// \brief Drops ownership of all but the current pool buffers, however the buffers are kept alive by
	/// callback_pool_function objects using them.
	void shrink() noexcept {
		std::lock_guard<std::mutex> lock(pool_mutex);
		stashed_buffers.clear();
	}
	/// Returns the total (not just free) space in the buffers of the pool.
	size_t capacity() const noexcept;
};

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_CALLBACK_POOL_HPP_ */
