/*
 * Multi-Core Engine project
 * File /multicore_engine_renderer/include/mce/graphics/simple_uniform_buffer_pool.hpp
 * Copyright 2017 by Stefan Bodenschatz
 */

#ifndef MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_POOL_HPP_
#define MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_POOL_HPP_

#include <algorithm>
#include <mce/graphics/simple_uniform_buffer.hpp>
#include <numeric>
#include <vector>

namespace mce {
namespace graphics {

/// \brief Provides a growing counterpart of simple_uniform_buffer by managing a multiple such buffers and
/// adding a new one if the available capacity is depleted.
class simple_uniform_buffer_pool {
	device* dev_;
	device_memory_manager_interface* mem_mgr_;
	destruction_queue_manager* dqm_;
	vk::DeviceSize buffer_size_;
	std::vector<simple_uniform_buffer> buffers_;

public:
	/// \brief Creates a simple_uniform_buffer_pool for buffers of the given size on the given device
	/// allocating memory from the given memory manager and using the given destruction_queue_manager to defer
	/// freeing resources.
	simple_uniform_buffer_pool(device& dev, device_memory_manager_interface& mem_mgr,
							   destruction_queue_manager* destruction_manager, vk::DeviceSize buffer_size);
	/// \brief Destroys the pool and all buffers in it, releasing the underlying resources to the
	/// destruction_queue_manager given at construction.
	~simple_uniform_buffer_pool() = default;

	/// Forbids copying.
	simple_uniform_buffer_pool(const simple_uniform_buffer_pool&) = delete;
	/// Forbids copying.
	simple_uniform_buffer_pool& operator=(const simple_uniform_buffer_pool&) = delete;
	/// Allows moving.
	simple_uniform_buffer_pool(simple_uniform_buffer_pool&&) noexcept = default;
	/// Allows moving.
	simple_uniform_buffer_pool& operator=(simple_uniform_buffer_pool&&) noexcept = default;

	/// \brief Searches for a buffer that has enough space left for the given data value and stores it there
	/// or allocates a new buffer and stores the value there if no existing buffer can fit it.
	template <typename T, typename = std::enable_if<detail::uniform_buffer_is_element_compatible<T>::value>>
	vk::DescriptorBufferInfo store(const T& value) {
		auto it = std::find_if(buffers_.begin(), buffers_.end(),
							   [&value](simple_uniform_buffer& b) { return b.can_fit(value); });
		if(it == buffers_.end()) {
			buffers_.emplace_back(*dev_, *mem_mgr_, dqm_, buffer_size_);
			it = buffers_.end() - 1;
		}
		return it->store(value);
	}
	/// Resets all contained buffers to an empty state, invalidating all stored data.
	void reset() {
		for(auto& b : buffers_) {
			b.reset();
		}
	}
	/// \brief Flushes all contained buffers to ensure the data is visible to the device even if the memory is
	/// not mapped host-coherently.
	void flush() {
		for(auto& b : buffers_) {
			b.flush();
		}
	}
	/// Returns the sum of the available space in all contained buffers.
	vk::DeviceSize available_space() const {
		return std::accumulate(
				buffers_.begin(), buffers_.end(), 0ull,
				[](vk::DeviceSize s, const simple_uniform_buffer& b) { return s + b.available_space(); });
	}
};

} /* namespace graphics */
} /* namespace mce */

#endif /* MCE_GRAPHICS_SIMPLE_UNIFORM_BUFFER_POOL_HPP_ */
