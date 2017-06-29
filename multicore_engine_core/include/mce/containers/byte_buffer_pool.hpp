/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/byte_buffer_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_CONTAINERS_BYTE_BUFFER_POOL_HPP_
#define MCE_CONTAINERS_BYTE_BUFFER_POOL_HPP_

#include <atomic>
#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4146)
#endif
#include <boost/rational.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace mce {
namespace containers {

namespace detail {

class byte_buffer_pool_buffer {
	char* data_;
	size_t size_;
	std::atomic<size_t> ref_count_{0};

public:
	byte_buffer_pool_buffer(char* data, size_t size) noexcept : data_{data}, size_{size} {}

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

class byte_buffer_pool_buffer_deleter {
	char* raw_buffer;

public:
	explicit byte_buffer_pool_buffer_deleter(char* raw_buffer) noexcept : raw_buffer{raw_buffer} {}

	void operator()(byte_buffer_pool_buffer* b) noexcept {
		b->~byte_buffer_pool_buffer();
		delete[] raw_buffer;
	}
};

} // namespace detail

class pooled_byte_buffer_ptr {
	std::shared_ptr<detail::byte_buffer_pool_buffer> pool_buffer_;
	char* data_;
	size_t size_;

	pooled_byte_buffer_ptr(std::shared_ptr<detail::byte_buffer_pool_buffer>&& pool_buffer, char* data,
						   size_t size)
			: pool_buffer_{std::move(pool_buffer)}, data_{data}, size_{size} {}
	pooled_byte_buffer_ptr(const std::shared_ptr<detail::byte_buffer_pool_buffer>& pool_buffer, char* data,
						   size_t size)
			: pool_buffer_{pool_buffer}, data_{data}, size_{size} {}

	friend class byte_buffer_pool;

public:
	pooled_byte_buffer_ptr(const pooled_byte_buffer_ptr& other) noexcept : pool_buffer_{other.pool_buffer_},
																		   data_{other.data_},
																		   size_{other.size_} {
		if(pool_buffer_) pool_buffer_->increment_ref_count();
	}
	pooled_byte_buffer_ptr& operator=(const pooled_byte_buffer_ptr& other) noexcept {
		if(this == &other) return *this;
		if(other.pool_buffer_) other.pool_buffer_->increment_ref_count();
		if(pool_buffer_) pool_buffer_->decrement_ref_count();
		pool_buffer_ = other.pool_buffer_;
		data_ = other.data_;
		size_ = other.size_;
		return *this;
	}
	pooled_byte_buffer_ptr(pooled_byte_buffer_ptr&& other) noexcept
			: pool_buffer_{std::move(other.pool_buffer_)},
			  data_{other.data_},
			  size_{other.size_} {
		other.reset();
	}
	pooled_byte_buffer_ptr& operator=(pooled_byte_buffer_ptr&& other) noexcept {
		using std::swap;
		if(this == &other) return *this;
		swap(pool_buffer_, other.pool_buffer_);
		swap(data_, other.data_);
		swap(size_, other.size_);
		other.reset();
		return *this;
	}

	pooled_byte_buffer_ptr() noexcept : data_{nullptr}, size_{0} {}
	~pooled_byte_buffer_ptr() noexcept {
		reset();
	}

	void reset() noexcept {
		if(pool_buffer_) pool_buffer_->decrement_ref_count();
		pool_buffer_ = nullptr;
		data_ = nullptr;
		size_ = 0;
	}

	const char* data() const {
		return data_;
	}

	char* data() {
		return data_;
	}

	size_t size() const {
		return size_;
	}
};

class byte_buffer_pool {
	std::shared_ptr<detail::byte_buffer_pool_buffer> current_pool_buffer;
	std::vector<std::shared_ptr<detail::byte_buffer_pool_buffer>> stashed_pool_buffers;
	size_t current_pool_buffer_offset = 0;
	mutable std::mutex pool_mutex;
	size_t pool_buffer_size_;
	size_t min_slots_;
	boost::rational<size_t> growth_factor_;

	void reallocate(size_t buff_size);
	void* try_alloc_buffer_block(size_t size) const noexcept;

public:
	byte_buffer_pool(size_t buffer_size = 0x100000, size_t min_slots = 0x10,
					 boost::rational<size_t> growth_factor = {3u, 2u});
	byte_buffer_pool(byte_buffer_pool&& other) noexcept;
	byte_buffer_pool& operator=(byte_buffer_pool&& other) noexcept;
	pooled_byte_buffer_ptr allocate_buffer(size_t size);
	void release_resources() noexcept;
	size_t capacity() const noexcept;
};

} /* namespace containers */
} /* namespace mce */

#endif /* MCE_CONTAINERS_BYTE_BUFFER_POOL_HPP_ */
