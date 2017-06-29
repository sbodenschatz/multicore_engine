/*
 * Multi-Core Engine project
 * File /multicore_engine_core/include/mce/containers/byte_buffer_pool.cpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#include <mce/containers/byte_buffer_pool.hpp>
#include <numeric>

namespace mce {
namespace containers {

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
