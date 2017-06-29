/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/byte_buffer_pool.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <algorithm>
#include <cassert>
#include <mce/containers/byte_buffer_pool.hpp>
#include <mce/memory/align.hpp>
#include <mce/util/math_tools.hpp>
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
void byte_buffer_pool::reallocate(size_t buff_size) {
	// Stash current buffer (full or otherwise unusable when this is called)
	if(current_pool_buffer) stashed_pool_buffers.push_back(std::move(current_pool_buffer));
	current_pool_buffer_offset = 0;
	if(stashed_pool_buffers.size() > 1) {
		// Try to reclaim existing buffer
		auto it = std::find_if(stashed_pool_buffers.begin(), stashed_pool_buffers.end(),
							   [this, buff_size](const std::shared_ptr<detail::byte_buffer_pool_buffer>& b) {
								   return b->size() >= min_slots_ * buff_size && b->ref_count() == 0;
							   });
		if(it != stashed_pool_buffers.end()) {
			current_pool_buffer = std::move(*it);
			stashed_pool_buffers.erase(it);
			return;
		}
	}
	using util::ceil;
	// Create new buffer
	pool_buffer_size_ = ceil(pool_buffer_size_ * growth_factor_);
	if(pool_buffer_size_ < min_slots_ * buff_size) {
		pool_buffer_size_ = min_slots_ * buff_size;
	}
	auto raw_size = pool_buffer_size_ + sizeof(detail::byte_buffer_pool_buffer) +
					alignof(detail::byte_buffer_pool_buffer);
	auto tmp = new char[raw_size];
	void* tmp2 = tmp;
	auto space = sizeof(detail::byte_buffer_pool_buffer) + alignof(detail::byte_buffer_pool_buffer);
	auto buffer_header = reinterpret_cast<detail::byte_buffer_pool_buffer*>(memory::align(
			alignof(detail::byte_buffer_pool_buffer), sizeof(detail::byte_buffer_pool_buffer), tmp2, space));
	assert(buffer_header);

	auto buffer_size = (tmp + raw_size) - reinterpret_cast<char*>(buffer_header + 1);
	new(buffer_header)
			detail::byte_buffer_pool_buffer(reinterpret_cast<char*>(buffer_header + 1), buffer_size);
	current_pool_buffer = std::shared_ptr<detail::byte_buffer_pool_buffer>(
			buffer_header, detail::byte_buffer_pool_buffer_deleter(tmp));
}

void* byte_buffer_pool::try_alloc_buffer_block(size_t size) const noexcept {
	if(!current_pool_buffer) return nullptr;
	if(current_pool_buffer->size() < size) return nullptr;
	void* tmp = current_pool_buffer->data() + current_pool_buffer_offset;
	auto space = current_pool_buffer->size() - current_pool_buffer_offset;
	if(space < size) return nullptr;
	return tmp;
}

pooled_byte_buffer_ptr byte_buffer_pool::allocate_buffer(size_t size) {
	std::lock_guard<std::mutex> lock(pool_mutex);
	auto loc = try_alloc_buffer_block(size);
	if(!loc) {
		reallocate(size);
		loc = try_alloc_buffer_block(size);
		assert(loc);
	}
	current_pool_buffer->increment_ref_count();
	current_pool_buffer_offset = (reinterpret_cast<char*>(loc) + size) - current_pool_buffer->data();
	return pooled_byte_buffer_ptr(current_pool_buffer, reinterpret_cast<char*>(loc), size);
}

} /* namespace containers */
} /* namespace mce */
