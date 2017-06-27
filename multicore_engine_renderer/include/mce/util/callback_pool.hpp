/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/util/callback_pool.hpp
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
	callback_pool_function() noexcept;
	callback_pool_function(callback_pool_function&& other) noexcept
			: internal_function_object{std::move(other.internal_function_object)} {
		other.internal_function_object = nullptr;
	}
	callback_pool_function& operator=(callback_pool_function&& other) noexcept {
		using std::swap;
		swap(internal_function_object, other.internal_function_object);
		other.internal_function_object.reset();
		return *this;
	}

	R operator()(Args... args) const {
		const detail::callback_pool_function_iface<R(Args...)>* f = internal_function_object.get();
		if(!f) throw std::bad_function_call();
		return (*f)(std::forward<Args>(args)...);
	}
	R operator()(Args... args) {
		detail::callback_pool_function_iface<R(Args...)>* f = internal_function_object.get();
		if(!f) throw std::bad_function_call();
		return (*f)(std::forward<Args>(args)...);
	}
	operator bool() noexcept {
		return internal_function_object;
	}
};

class callback_pool {
	std::shared_ptr<detail::callback_pool_buffer> current_buffer;
	std::vector<std::shared_ptr<detail::callback_pool_buffer>> stashed_buffers;
	size_t current_buffer_offset = 0;
	mutable std::mutex pool_mutex;
	static constexpr size_t min_slots = 10;
	static constexpr size_t growth_factor = 2;
	static constexpr size_t min_buffer_size = 1 << 24;

	void reallocate(size_t alloc_size) {
		// Stash current buffer (full or otherwise unusable when this is called)
		if(current_buffer) stashed_buffers.push_back(std::move(current_buffer));
		current_buffer_offset = 0;
		if(stashed_buffers.size() > 1) {
			// Try to reclaim existing buffer
			auto it = std::find_if(stashed_buffers.begin(), stashed_buffers.end(),
								   [alloc_size](const std::shared_ptr<detail::callback_pool_buffer>& b) {
									   return b->size() >= alloc_size && b->ref_count() == 0;
								   });
			if(it != stashed_buffers.end()) {
				current_buffer = std::move(*it);
				stashed_buffers.erase(it);
				return;
			}
		}
		// Create new buffer
		auto raw_size = growth_factor * alloc_size + sizeof(detail::callback_pool_buffer) +
						alignof(detail::callback_pool_buffer);
		auto tmp = new char[raw_size];
		void* tmp2 = tmp;
		auto space = sizeof(detail::callback_pool_buffer) + alignof(detail::callback_pool_buffer);
		auto buffer_header = reinterpret_cast<detail::callback_pool_buffer*>(memory::align(
				alignof(detail::callback_pool_buffer), sizeof(detail::callback_pool_buffer), tmp2, space));
		assert(buffer_header);

		auto buffer_size = (tmp + raw_size) - reinterpret_cast<char*>(buffer_header + 1);
		new(buffer_header)
				detail::callback_pool_buffer(reinterpret_cast<char*>(buffer_header + 1), buffer_size);
		current_buffer = std::shared_ptr<detail::callback_pool_buffer>(
				buffer_header, detail::callback_pool_buffer_deleter(tmp));
	}

	void* try_alloc_obj_block(size_t size, size_t alignment) {
		if(!current_buffer) return nullptr;
		if(current_buffer->size() < size) return nullptr;
		void* tmp = current_buffer->data() + current_buffer_offset;
		auto space = current_buffer->size() - current_buffer_offset;
		return memory::align(alignment, size, tmp, space);
	}

	size_t curr_buf_cap() const {
		return current_buffer ? current_buffer->size() : size_t(0);
	}

public:
	callback_pool() {}

	callback_pool(callback_pool&& other) noexcept {
		using std::swap;
		std::lock(pool_mutex, other.pool_mutex);
		std::lock_guard<std::mutex> l1(pool_mutex, std::adopt_lock);
		std::lock_guard<std::mutex> l2(other.pool_mutex, std::adopt_lock);
		swap(current_buffer, other.current_buffer);
		swap(stashed_buffers, other.stashed_buffers);
		swap(current_buffer_offset, other.current_buffer_offset);
	}
	callback_pool& operator=(callback_pool&& other) noexcept {
		using std::swap;
		std::lock(pool_mutex, other.pool_mutex);
		std::lock_guard<std::mutex> l1(pool_mutex, std::adopt_lock);
		std::lock_guard<std::mutex> l2(other.pool_mutex, std::adopt_lock);
		swap(current_buffer, other.current_buffer);
		swap(stashed_buffers, other.stashed_buffers);
		swap(current_buffer_offset, other.current_buffer_offset);
		return *this;
	}

	template <typename Signature, typename F>
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
		auto fun_ptr = new(loc) fun(std::forward<F>(f));
		current_buffer_offset = reinterpret_cast<char*>(fun_ptr + 1) - current_buffer->data();
		return callback_pool_function<Signature>(fun_ptr, current_buffer);
	}

	void reserve(size_t size) {
		std::lock_guard<std::mutex> lock(pool_mutex);
		if(!current_buffer || current_buffer->size() < size) {
			reallocate(size);
		}
	}
	void shrink() {
		std::lock_guard<std::mutex> lock(pool_mutex);
		stashed_buffers.clear();
	}
	size_t capacity() const {
		std::lock_guard<std::mutex> lock(pool_mutex);
		size_t tmp = 0;
		if(current_buffer) tmp = current_buffer->size();
		return std::accumulate(stashed_buffers.begin(), stashed_buffers.end(), tmp,
							   [](size_t s, const std::shared_ptr<detail::callback_pool_buffer>& b) {
								   return s + b->size();
							   });
	}
};

} // namespace util
} // namespace mce

#endif /* MCE_UTIL_CALLBACK_POOL_HPP_ */
