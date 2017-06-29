/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/byte_buffer_pool.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/containers/byte_buffer_pool.hpp>
#include <numeric>

namespace mce {
namespace containers {

byte_buffer_pool::byte_buffer_pool(size_t buffer_size, size_t min_slots,
								   boost::rational<size_t> growth_factor)
		: pool_buffer_size_{buffer_size}, min_slots_{min_slots}, growth_factor_{1u} {
	// Allocate two buffers with the given buffer size.
	reallocate(1);
	reallocate(1);
	growth_factor_ = growth_factor;
}
byte_buffer_pool::byte_buffer_pool(byte_buffer_pool&& other) noexcept {
	using std::swap;
	std::lock(pool_mutex, other.pool_mutex);
	std::lock_guard<std::mutex> l1(pool_mutex, std::adopt_lock);
	std::lock_guard<std::mutex> l2(other.pool_mutex, std::adopt_lock);
	swap(current_pool_buffer, other.current_pool_buffer);
	swap(stashed_pool_buffers, other.stashed_pool_buffers);
	swap(current_pool_buffer_offset, other.current_pool_buffer_offset);
	swap(pool_buffer_size_, other.pool_buffer_size_);
	swap(min_slots_, other.min_slots_);
	swap(growth_factor_, other.growth_factor_);
}
byte_buffer_pool& byte_buffer_pool::operator=(byte_buffer_pool&& other) noexcept {
	using std::swap;
	std::lock(pool_mutex, other.pool_mutex);
	std::lock_guard<std::mutex> l1(pool_mutex, std::adopt_lock);
	std::lock_guard<std::mutex> l2(other.pool_mutex, std::adopt_lock);
	swap(current_pool_buffer, other.current_pool_buffer);
	swap(stashed_pool_buffers, other.stashed_pool_buffers);
	swap(current_pool_buffer_offset, other.current_pool_buffer_offset);
	swap(pool_buffer_size_, other.pool_buffer_size_);
	swap(min_slots_, other.min_slots_);
	swap(growth_factor_, other.growth_factor_);
	return *this;
}

void byte_buffer_pool::release_resources() noexcept {
	std::lock_guard<std::mutex> lock(pool_mutex);
	current_pool_buffer.reset();
	current_pool_buffer_offset = 0;
	stashed_pool_buffers.clear();
}

size_t byte_buffer_pool::capacity() const noexcept {
	std::lock_guard<std::mutex> lock(pool_mutex);
	size_t tmp = 0;
	if(current_pool_buffer) tmp = current_pool_buffer->size();
	return std::accumulate(stashed_pool_buffers.begin(), stashed_pool_buffers.end(), tmp,
						   [](size_t s, const std::shared_ptr<detail::byte_buffer_pool_buffer>& b) {
							   return s + b->size();
						   });
}

} /* namespace containers */
} /* namespace mce */
