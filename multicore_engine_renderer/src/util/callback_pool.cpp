/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/src/util/callback_pool.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/util/callback_pool.hpp>

namespace mce {
namespace util {

void callback_pool::reallocate(size_t alloc_size) {
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
	new(buffer_header) detail::callback_pool_buffer(reinterpret_cast<char*>(buffer_header + 1), buffer_size);
	current_buffer = std::shared_ptr<detail::callback_pool_buffer>(buffer_header,
																   detail::callback_pool_buffer_deleter(tmp));
}

void* callback_pool::try_alloc_obj_block(size_t size, size_t alignment) const noexcept {
	if(!current_buffer) return nullptr;
	if(current_buffer->size() < size) return nullptr;
	void* tmp = current_buffer->data() + current_buffer_offset;
	auto space = current_buffer->size() - current_buffer_offset;
	return memory::align(alignment, size, tmp, space);
}

callback_pool::callback_pool(callback_pool&& other) noexcept {
	using std::swap;
	std::lock(pool_mutex, other.pool_mutex);
	std::lock_guard<std::mutex> l1(pool_mutex, std::adopt_lock);
	std::lock_guard<std::mutex> l2(other.pool_mutex, std::adopt_lock);
	swap(current_buffer, other.current_buffer);
	swap(stashed_buffers, other.stashed_buffers);
	swap(current_buffer_offset, other.current_buffer_offset);
}
callback_pool& callback_pool::operator=(callback_pool&& other) noexcept {
	using std::swap;
	std::lock(pool_mutex, other.pool_mutex);
	std::lock_guard<std::mutex> l1(pool_mutex, std::adopt_lock);
	std::lock_guard<std::mutex> l2(other.pool_mutex, std::adopt_lock);
	swap(current_buffer, other.current_buffer);
	swap(stashed_buffers, other.stashed_buffers);
	swap(current_buffer_offset, other.current_buffer_offset);
	return *this;
}
size_t callback_pool::capacity() const noexcept {
	std::lock_guard<std::mutex> lock(pool_mutex);
	size_t tmp = 0;
	if(current_buffer) tmp = current_buffer->size();
	return std::accumulate(
			stashed_buffers.begin(), stashed_buffers.end(), tmp,
			[](size_t s, const std::shared_ptr<detail::callback_pool_buffer>& b) { return s + b->size(); });
}

} // namespace util
} // namespace mce
